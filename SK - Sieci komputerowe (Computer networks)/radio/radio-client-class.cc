#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
#include <regex>
#include <poll.h>
#include <chrono>
#include <utility>
#include <thread>
#include <mutex>

#include <iostream>

#include "utils.h"
#include "exceptions.h"
#include "utils.h"
#include "radio-client-class.h"
#include "radio-info-class.h"

/*
  Symbole poszczególnych parametrów związanych z podstawowym
  wywołaniem programu.
*/
namespace Param {
  const char HOST = 'H';
  const char PORT = 'P';
  const char TELNET_PORT = 'p';
  const char TIMEOUT = 'T';

  const int obligatory_count = 3;
  const int count = 4;
}

namespace Menu {
  const std::string SEARCH_OPT = "Szukaj pośrednika";
  const std::string FINISH_OPT = "Koniec";
  const std::string PREFIX = "Pośrednik";
  const char NOW_PLAYING = '*';
  const char CURRENT_OPTION = '<';

  const std::string UP = "\033[A";
  const std::string DOWN = "\033[B";

  const char CR = '\r';
}

namespace {
  const int TTL_VALUE = 4;
  const std::string TITLE_HEADER = "StreamTitle";

  /* Wzorzec dla parametrów */
  const std::regex param_structure{"^-.$"};

  /* Sprawdza, czy liczba parametrów jest odpowiednia */
  bool check_param_count(int paramc) {
    int paramc_without_name = paramc - 1;

    return paramc_without_name >= 2 * Param::obligatory_count &&
           paramc_without_name <= 2 * Param::count &&
           paramc_without_name % 2 == 0;
  }

  /* Wzorce dla poszczegónych parametrów */
  const std::regex not_empty{"^.+$"};
  const std::regex decimal{"^0$|^[1-9]\\d*$"};
  const std::regex timeout_structure{"^[1-9]\\d*$"};

  /* Sprawdza, czy nie podano już wcześniej danego parametru oraz
     czy jest on dopuszczalny, czyli zgodny ze swoim wzorcem.
  */
  void validate_param(char* parameters[], int p, bool param_set,
                      const std::string& param_name, const std::regex& re) {
    if (param_set)
      throw std::invalid_argument(param_name + " param set more than once");

    if (!std::regex_match(parameters[p + 1], re))
      throw std::invalid_argument("Invalid " + param_name + " param given");
  }

  std::mutex mut;
  std::mutex sock_mut;
  bool KEEPALIVE = true;

  const socklen_t snda_len = (socklen_t) sizeof(struct sockaddr_in);

  ssize_t safe_send(int sock, char* buf, size_t len, const struct sockaddr_in& address) {
    std::lock_guard<std::mutex> lock(sock_mut);

    return sendto(sock, buf, len, 0, (struct sockaddr*) &address, snda_len);
  }

  void keepaliver(int sock, std::vector<char> msg, struct sockaddr_in address) {
    mut.lock();

    while (KEEPALIVE) {
      mut.unlock();

      safe_send(sock, msg.data(), msg.size(), address);
      std::this_thread::sleep_for(std::chrono::milliseconds(3500));

      mut.lock();
    }
  }

  void stop_keepaliver() {
    std::lock_guard<std::mutex> lock(mut);

    KEEPALIVE = false;
  }

  namespace poll_info {
    const int PROXY_SOCK = 0;
    const int TELNET_SOCK = 1;

    const int SOCKET_NUM = 2;
  }
}

bool RadioClient::invalid_param_combo() {
  return !radio::is_set(host) || !radio::is_set(port) || !radio::is_set(telnet_port);
}

void RadioClient::read_params(int paramc, char* parameters[]) {
  bool timeout_set;

  timeout_set = false;

  for (int p = 1; p < paramc; p += 2) {
    if (!std::regex_match(parameters[p], param_structure))
      throw std::invalid_argument(std::string("Unknown argument: ") + parameters[p]);

    // Czytamy sam symbol parametru bez myślnika
    char raw_param = parameters[p][1];

    switch(raw_param) {
      case Param::HOST:
        validate_param(parameters, p, radio::is_set(host), "host", not_empty);

        host = parameters[p + 1];
        break;
      case Param::PORT:
        validate_param(parameters, p, radio::is_set(port), "port", decimal);
        
        port = std::stoi(parameters[p + 1]);
        break;
      case Param::TELNET_PORT:
        validate_param(parameters, p, radio::is_set(telnet_port), "telnet port", decimal);

        telnet_port = std::stoi(parameters[p + 1]);
        break;
      case Param::TIMEOUT:
        validate_param(parameters, p, timeout_set, "timeout", timeout_structure);

        timeout = std::stoi(parameters[p + 1]);
        timeout_set = true;
        break;
      default:
        throw std::invalid_argument(std::string("Unknown param: ") + parameters[p]);
    }
  }

  if (invalid_param_combo())
    throw std::invalid_argument("some essential parameter was not given");
}

void RadioClient::open_telnet_socket() {
  struct sockaddr_in server;

  telnet_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (telnet_sock < 0)
    throw SystemError{"socket"};

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons((in_port_t) telnet_port);

  if (bind(telnet_sock, (struct sockaddr *) &server, sizeof(server)) < 0)
    throw SystemError{"bind"};

  if (listen(telnet_sock, 5) < 0)
    throw SystemError{"listen"};
}

void RadioClient::open_udp_socket() {
  int optval;
  struct sockaddr_in server_address;
  struct timeval to;

  to.tv_sec = timeout;
  to.tv_usec = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons((in_port_t) port);
  if (inet_aton(host.c_str(), &server_address.sin_addr) == 0)
    throw SystemError{"inet_aton - invalid multicast address"};

  proxy_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (proxy_sock < 0)
    throw SystemError{"socket"};

  optval = 1;
  if (setsockopt(proxy_sock, SOL_SOCKET, SO_BROADCAST, (void*)&optval, sizeof optval) < 0)
    throw SystemError{"setsockopt broadcast"};

  optval = TTL_VALUE;
  if (setsockopt(proxy_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&optval, sizeof optval) < 0)
    throw SystemError{"setsockopt multicast ttl"};

  if (setsockopt(proxy_sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&to, sizeof(to)) < 0)
    throw SystemError{"setsockopt"};

  search_addr = server_address;
}

RadioClient::RadioClient(int paramc, char* parameters[])
                        : menu_options{Menu::SEARCH_OPT, Menu::FINISH_OPT} {
  if (!check_param_count(paramc))
    throw std::invalid_argument("Wrong number of arguments");

  read_params(paramc, parameters);

  open_telnet_socket();
  open_udp_socket();

  radio::construct_msg(msg::DISCOVER, discover_msg, radio::EMPTY_STRING);
  radio::construct_msg(msg::KEEPALIVE, keepalive_msg, radio::EMPTY_STRING);
}

void RadioClient::next() {
  cur_option = (cur_option + 1) % menu_options.size();
}

void RadioClient::prev() {
  cur_option = (cur_option - 1) % menu_options.size();
}

void RadioClient::del_menu_option(size_t pos) {
  menu_options.erase(menu_options.begin() + pos);
  if (pos < cur_option)
    prev();
}

void RadioClient::update_radios() {
  for (size_t pos = 1; pos < menu_options.size() - 1;) {
    auto radio_it = radios.find(menu_options[pos]);

    if (radio_it == radios.end()) {
      del_menu_option(pos);
    }
    else {
      auto& cur_radio = *(radio_it->second);
      if (!cur_radio.is_alive(timeout, event_time)) {
        /*if (radio_it == chosen_radio_it)
          chosen_radio_it = radios.end();*/
        del_menu_option(pos);
        radios.erase(radio_it);
      }
      else {
        pos++;
      }
    }
  }
}

void RadioClient::show_menu() {
  update_radios();

  const std::string& current_radio_name = chosen_radio_it != radios.end() ?
                                          chosen_radio_it->first :
                                          radio::EMPTY_STRING;

  if (system("clear") != 0)
    throw SystemError{"clear"};

  for (size_t pos = 0; pos < menu_options.size(); pos++) {
    if (0 < pos && pos < menu_options.size() - 1) {
      fprintf(stderr, "%s ", Menu::PREFIX.c_str());
    }

    fprintf(stderr, "%s", menu_options[pos].c_str());

    if (0 < pos && pos < menu_options.size() - 1 &&
        menu_options[pos] == current_radio_name) {
      fprintf(stderr, " %c", Menu::NOW_PLAYING);
    }

    if (pos == cur_option)
      fprintf(stderr, " %c", Menu::CURRENT_OPTION);

    fprintf(stderr, "\n");
  }

  if (chosen_radio_it != radios.end() && radio::is_set(metadata))
    fprintf(stderr, "%s\n", metadata.c_str());
}

void RadioClient::run_menu_option() {
  if (cur_option == 0) {
    ssize_t ret;

    ret = safe_send(proxy_sock, discover_msg.data(), discover_msg.size(), search_addr);
    if (ret < 0)
      throw SystemError{"sendto"};
  }
  else if (cur_option == menu_options.size() - 1) {
    STOP = true;
  }
  else {
    if (chosen_radio_it != radios.end())
      stop_keepaliver();

    chosen_radio_it = radios.find(menu_options[cur_option]);
    if (chosen_radio_it != radios.end()) {
      auto& radio_ref = (*(chosen_radio_it->second));
      const auto& radio_addr = radio_ref.get_address();
      safe_send(proxy_sock, discover_msg.data(), discover_msg.size(), radio_addr);

      std::thread sender{keepaliver, proxy_sock, keepalive_msg, radio_addr};
      sender.detach();
    }

    show_menu();
  }
}

/*
  Przy założeniu że uruchamiamy telnet w trybie character.
*/
void RadioClient::handle_key_press() {
  ssize_t read_bytes;

  read_bytes = read(rem_cont_sock, buf, Menu::UP.length());
  if (read_bytes <= 0) {
    CONTROLLED = false;
    return;
  }

  buf[read_bytes] = '\0';

  if (Menu::UP.compare(buf) == 0) {
    next();
    show_menu();
  }
  else if (Menu::DOWN.compare(buf) == 0) {
    prev();
    show_menu();
  }
  else if (buf[0] == Menu::CR) {
    run_menu_option();
  }
}

void RadioClient::maybe_add_option(const std::string& name) {
  size_t pos;

  bool found = false;

  for (pos = 1; !found && pos < menu_options.size(); pos++) {
    if (name == menu_options[pos])
      found = true;
  }

  if (!found)
    menu_options.insert(menu_options.end() - 1, name);
}

void RadioClient::handle_iam(const struct sockaddr_in& radio_address) {
  std::string name{buf + radio::HEADER_LENGTH};

  auto radio_it = radios.find(name);

  if (radio_it == radios.end())
    radios.emplace(name, std::move(std::make_unique<RadioInfo>(name, radio_address)));
  else
    (*(radio_it->second)).update_time(event_time);

  maybe_add_option(name);
}

void RadioClient::handle_audio(size_t length, const struct sockaddr_in& radio_address) {
  if (chosen_radio_it == radios.end())
    return;

  auto& radio_ref = *(chosen_radio_it->second);

  if (radio_ref.equals(radio_address)) {
    radio_ref.update_time(event_time);
    fwrite(buf + radio::HEADER_LENGTH, 1, length, stdout);
  }
}

void RadioClient::parse_meta() {
  std::string meta_raw = buf + radio::HEADER_LENGTH;

  size_t title_header_pos = meta_raw.find(TITLE_HEADER);
  if (title_header_pos == std::string::npos)
    return;

  size_t semicolon_pos = meta_raw.find(';', title_header_pos);
  if (semicolon_pos == std::string::npos)
    return;

  size_t meta_start = title_header_pos + TITLE_HEADER.length() + strlen("=\'");
  size_t meta_length = semicolon_pos - meta_start - 1;

  if (meta_length > 0)
    metadata = meta_raw.substr(meta_start, meta_length);
}

void RadioClient::handle_meta(const struct sockaddr_in& radio_address) {
  if (chosen_radio_it == radios.end())
    return;

  auto& radio_ref = *(chosen_radio_it->second);

  if (radio_ref.equals(radio_address)) {
    radio_ref.update_time(event_time);
    parse_meta();
  }
}

void RadioClient::interpret_msg(size_t read_bytes, const struct sockaddr_in& radio_address) {
  uint16_t type, length;

  type = ntohs(*(uint16_t*)buf);
  length = ntohs(*(uint16_t*)(buf + sizeof(type)));

  if (length == 0 || read_bytes - radio::HEADER_LENGTH != length)
    return;

  buf[read_bytes] = '\0';

  switch (type) {
    case msg::IAM:
      handle_iam(radio_address);
      break;
    case msg::AUDIO:
      handle_audio(length, radio_address);
      break;
    case msg::METADATA:
    handle_meta(radio_address);
      break;
    default:
      break;
  }
}

void RadioClient::handle_radio_msg() {
  ssize_t read_bytes;
  socklen_t rcva_len;
  struct sockaddr_in radio_address;

  rcva_len = (socklen_t) sizeof(radio_address);
  read_bytes = recvfrom(proxy_sock, buf, sizeof(buf), 0,
                        (struct sockaddr*) &radio_address, &rcva_len);

  if (read_bytes < 0)
    return;

  if ((size_t) read_bytes > radio::HEADER_LENGTH)
    interpret_msg(read_bytes, radio_address);
}

void RadioClient::inspect_chosen_radio() {
  if (chosen_radio_it == radios.end())
    return;

  auto& radio_ref = *(chosen_radio_it->second);

  if (!radio_ref.is_alive(timeout, event_time)) {
    stop_keepaliver();
    radios.erase(chosen_radio_it);
    chosen_radio_it = radios.end();
    metadata = radio::EMPTY_STRING;
  }
}

void RadioClient::start() {
  int ret;
  struct pollfd sockets[poll_info::SOCKET_NUM];

  for (auto& poll_sock : sockets) {
    poll_sock.events = POLLIN;
    poll_sock.revents = 0;
  }

  sockets[poll_info::PROXY_SOCK].fd = proxy_sock;

  while (!STOP) {
    if (system("clear") != 0)
      throw SystemError{"clear"};

    // Czekamy na połączenie telnet.
    rem_cont_sock = accept(telnet_sock, (struct sockaddr*)nullptr, (socklen_t*)nullptr);
    if (rem_cont_sock == -1 && !radio::interrupted())
      throw SystemError{"accept"};

    sockets[poll_info::TELNET_SOCK].fd = rem_cont_sock;

    if (!STOP) {
      show_menu();
      CONTROLLED = true;
    }

    while (!STOP && CONTROLLED) {
      for (auto& poll_sock : sockets)
        poll_sock.revents = 0;

      ret = poll(sockets, poll_info::SOCKET_NUM, timeout * 1000);

      event_time = std::chrono::steady_clock::now();
      inspect_chosen_radio();

      if (ret == -1 && !radio::interrupted()) {
        throw std::runtime_error{"poll"};
      }
      else if (ret > 0) {
        if (!STOP && (sockets[poll_info::TELNET_SOCK].revents & POLLIN))
          handle_key_press();

        if (!STOP && (sockets[poll_info::PROXY_SOCK].revents & (POLLIN | POLLERR)))
          handle_radio_msg();
      }
      else if (ret == 0) {
        show_menu();
      }
    }

    if (radio::is_set(rem_cont_sock) && (rem_cont_sock) < 0)
      throw SystemError{"close"};

    rem_cont_sock = radio::NO_VALUE;
  }
}

void RadioClient::stop() {
  STOP = true;
}