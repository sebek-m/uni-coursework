#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
#include <regex>

#include <iostream>

#include "utils.h"
#include "proxy-initializer.h"
#include "exceptions.h"
#include "utils.h"
#include "proxy-class.h"
#include "outputter-class.h"
#include "client-handler-class.h"
#include "processor-class.h"

/*
  Symbole poszczególnych parametrów związanych z podstawowym
  wywołaniem programu.
*/
namespace Param {
  const char HOST = 'h';
  const char RESOURCE = 'r';
  const char PORT = 'p';
  const char METADATA = 'm';
  const char TIMEOUT = 't';

  const int obligatory_count = 3;
  const int count = 5;
}

/*
  Symbole poszczególnych parametrów związanych z wywołaniem
  programu w trybie pośrednika.
*/
namespace BrokerParam {
  const char PORT = 'P';
  const char MULTI = 'B';
  const char TIMEOUT = 'T';

  const int count = 3;
}

namespace {
  /* Wzorzec dla parametrów */
  const std::regex param_structure{"^-.$"};

  /* Sprawdza, czy liczba parametrów jest odpowiednia */
  bool check_param_count(int paramc) {
    int paramc_without_name = paramc - 1;

    return paramc_without_name >= 2 * Param::obligatory_count &&
           paramc_without_name <= 2 * (Param::count + BrokerParam::count) &&
           paramc_without_name % 2 == 0;
  }

  /* Wzorce dla poszczegónych parametrów */
  const std::regex not_empty{"^.+$"};
  const std::regex decimal{"^0$|^[1-9]\\d*$"};
  const std::regex metadata_structure{"^yes|no$"};
  const std::regex timeout_structure{"^[1-9]\\d*$"};

  /*
    Stałe do komunikacji przez HTTP
  */
  const std::string CRLF = "\r\n";
  const std::string OWS = " \t";

  const std::string version_str = "(ICY|HTTP\\/1\\.0|HTTP\\/1\\.1)";
  const std::regex VERSION_PREFIX{'^' + version_str + ' '};
  const std::regex ANY_STATUS_LINE{'^' + version_str + " .*"};
  const std::regex OK_STATUS_LINE{"^" + version_str + " 200 OK$"};

  const std::string NAME_HEADER = "icy-name";
  const std::string METAINT_HEADER = "icy-metaint";

  /*
    Struktura pomocnicza ze zmiennymi używanymi przy czytaniu linii statusu i nagłówków HTTP.
  */
  struct http_helper {
    char buf[radio::BUF_SIZE];
    std::string buf_str;

    bool status_line_read;
    bool empty_line;

    std::string name;
    ssize_t metaint;
  };

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

  /*
    Wysyła na dane gniazdo całą zawartość podanego bufora.
  */
  void send_whole_message(int sock, const char* msg, size_t msg_len) {
    ssize_t sent;
    size_t total_sent;

    sent = 0;
    total_sent = 0;

    while (total_sent < msg_len && !radio::interrupted()) {
      sent = write(sock, msg + total_sent, msg_len - total_sent);
      if (sent < 0 && !radio::interrupted())
        throw SystemError{"write"};

      total_sent += sent;
    }
  }

  /*
    Konwertuje podany napis do wersji zapisanej w całości małymi literami.
  */
  void str_to_lower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower); 
  }

  /*
    Zwraca napis bez spacji i tabów na początku i na końcu.
  */
  std::string trim_whitespaces(std::string& str) {
    size_t start, end;

    start = str.find_first_not_of(OWS);
    end = str.find_last_not_of(OWS);

    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
  }

  /*
    Zwraca wartość przeczytaną z nagłówka.
  */
  std::string extract_header_value(std::string& buf_str, size_t colon_pos, size_t CRLF_pos) {
    std::string after_colon = buf_str.substr(colon_pos + 1, CRLF_pos - (colon_pos + 1));

    return trim_whitespaces(after_colon);
  }

  /*
    Usuwa z początku buf_str linię odpowiedzi HTTP,
    czyli zawartość linii wraz z \r\n na jej końcu.
  */
  void trim_line(std::string& buf_str, size_t CRLF_pos) {
    buf_str.erase(0, CRLF_pos + CRLF.length());
  }

  /*
    Sprawdza, czy linia statusu jest jedną z dopuszczalnych.
    Jeśli nie, to rzuca odpowiedni wyjątek.
  */
  void process_status_line(struct http_helper& helper, std::string& line) {
    if (std::regex_match(line, OK_STATUS_LINE)) {
      helper.status_line_read = true;
    }
    else if (std::regex_match(line, ANY_STATUS_LINE)) {
      throw std::runtime_error("The radio has responded with a non-200 status: " +
                                std::regex_replace(line, VERSION_PREFIX, ""));
    }
    else {
      throw std::runtime_error("invalid status line format in http response");
    }
  }

  /*
    Sprawdza, czy linia jest prawidłową linią nagłówka i ewentualnie sczytuje
    z niej odpowiednie dane (nazwę radia lub stałą metaint).
  */
  void process_header_line(struct http_helper& helper, std::string& line, size_t CRLF_pos) {
    size_t colon_pos;

    str_to_lower(line);

    colon_pos = line.find(':');
    if (colon_pos == std::string::npos) // Linia niezgodna z formatem nagłówka
      throw std::runtime_error("invalid http response format");

    if (line.compare(0, colon_pos, NAME_HEADER) == 0) {
      if (radio::is_set(helper.name))
        throw std::runtime_error{"invalid HTTP response"};

      helper.name = extract_header_value(helper.buf_str, colon_pos, CRLF_pos);
    }
    else if (line.compare(0, colon_pos, METAINT_HEADER) == 0) {
      if (radio::is_set(helper.metaint))
        throw std::runtime_error{"invalid HTTP response"};

      std::string metaint_str = extract_header_value(helper.buf_str, colon_pos, CRLF_pos);

      if (!std::regex_match(metaint_str, decimal))
        throw std::runtime_error("invalid metaint in http response");

      helper.metaint = std::stoull(metaint_str);
    }
  }

  /*
    Przetwarza linie z nagłówkami, które znajdują się w buf_str,
    czyli odczytuje z nich odpowiednie dane, po czym je obcina.
  */
  void process_lines(struct http_helper& helper) {
    size_t CRLF_pos;

    while (!helper.empty_line && (CRLF_pos = helper.buf_str.find(CRLF)) != std::string::npos) {
      std::string line = helper.buf_str.substr(0, CRLF_pos);
      
      if (line.empty()) {
        helper.empty_line = true;
      }
      else if (!helper.status_line_read) {
        process_status_line(helper, line);
      }
      else {
        process_header_line(helper, line, CRLF_pos);
      }

      trim_line(helper.buf_str, CRLF_pos);
    }
  }

  /*
    Czyta z danego gniazda linię statusu i nagłówki HTTP.
    Wypełnia strukturę helper odczytanymi wartościami.
    W atrybucie buf_str zwracanej struktury mogą znajdować
    się pierwsze dane dźwiękowe przeczytane z gniazda,
    jeśli odebrano je wraz z nagłówkami.
  */
  void read_and_parse_response(int sock, struct http_helper& helper) {
    ssize_t r;

    helper.empty_line = false;
    helper.status_line_read = false;
    helper.metaint = radio::NO_VALUE;

    while (!helper.empty_line) {
      r = radio::read_and_handle(sock, helper.buf, radio::BUF_SIZE);

      helper.buf_str.append(helper.buf, r);

      process_lines(helper);
    }
  }
}

bool ProxyInitializer::invalid_param_combo(bool broker_timeout_set) {
  return !radio::is_set(host) || !radio::is_set(resource) || !radio::is_set(port) ||
         (!radio::is_set(broker_port) && (broker_timeout_set || radio::is_set(multi_address)));
}

void ProxyInitializer::read_params(int paramc, char* parameters[]) {
  bool timeout_set, broker_timeout_set, metadata_set;

  timeout_set = broker_timeout_set = metadata_set = false;

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
      case Param::RESOURCE:
        validate_param(parameters, p, radio::is_set(resource), "resource", not_empty);

        resource = parameters[p + 1];
        break;
      case Param::PORT:
        validate_param(parameters, p, radio::is_set(port), "port", decimal);
        
        port = parameters[p + 1];
        break;
      case Param::METADATA:
        validate_param(parameters, p, metadata_set, "metadata", metadata_structure);

        metadata = strcmp(parameters[p + 1], "yes") == 0;
        metadata_set = true;
        break;
      case Param::TIMEOUT:
        validate_param(parameters, p, timeout_set, "timeout", timeout_structure);

        timeout = std::stoi(parameters[p + 1]);
        timeout_set = true;
        break;
      case BrokerParam::PORT:
        validate_param(parameters, p, radio::is_set(broker_port), "broker port", decimal);
        
        broker_port = std::stoi(parameters[p + 1]);
        break;
      case BrokerParam::MULTI:
        validate_param(parameters, p, radio::is_set(multi_address), "multi address", not_empty);

        multi_address = parameters[p + 1];
        break;
      case BrokerParam::TIMEOUT:
        validate_param(parameters, p, broker_timeout_set, "broker timeout", timeout_structure);

        broker_timeout = std::stoi(parameters[p + 1]);
        broker_timeout_set = true;
        break;
      default:
        throw std::invalid_argument(std::string("Unknown param: ") + parameters[p]);
    }
  }

  if (invalid_param_combo(broker_timeout_set))
    throw std::invalid_argument("some essential parameter was not given");
}

std::string ProxyInitializer::build_request() {
  return "GET " + resource + " HTTP/1.1" + CRLF +
         "Host: " + host + CRLF +
         "User-Agent: SIKProxy" + CRLF +
         (metadata ? "Icy-MetaData: 1" + CRLF : "") +
         CRLF;
}

int ProxyInitializer::establish_connection() {
  int sock, rc;
  struct addrinfo addr_hints;
  struct addrinfo *addr_result;
  struct timeval to;

  to.tv_sec = timeout;
  to.tv_usec = 0;

  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    throw SystemError{"socket"};

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&to, sizeof(to)) < 0)
    throw SystemError{"setsockopt"};

  memset(&addr_hints, 0, sizeof(struct addrinfo));
  addr_hints.ai_flags = 0;
  addr_hints.ai_family = AF_INET;
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_protocol = IPPROTO_TCP;

  rc =  getaddrinfo(host.c_str(), port.c_str(), &addr_hints, &addr_result);
  if (rc != 0)
    throw GAIError{rc};

  if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0)
    throw SystemError{"connect"};

  freeaddrinfo(addr_result);

  return sock;
}

int ProxyInitializer::open_udp_socket() {
  int sock;
  struct sockaddr_in server_address;
  in_port_t udp_port;
  struct timeval to;

  to.tv_sec = broker_timeout;
  to.tv_usec = 0;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    throw SystemError{"socket"};

  /*
    Pomimo iż w przypadku działania proxy w trybie pośrednika timeout
    sprawdzany jest "manualnie" w ClientHandler, to ustawienie tej opcji
    nie zaszkodzi, a potencjalnie pozwoli na wykrycie niespodziewanego timeouta
    w metodzie ClientHandler::handle_client.
  */
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&to, sizeof(to)) < 0)
    throw SystemError{"setsockopt"};

  if (radio::is_set(multi_address)) {
    struct ip_mreq ip_mreq;

    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (inet_aton(multi_address.c_str(), &ip_mreq.imr_multiaddr) == 0)
      throw std::invalid_argument{"inet_aton - invalid multicast address"};

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&ip_mreq, sizeof(ip_mreq)) < 0)
      throw SystemError{"setsockopt"};
  }

  udp_port = (in_port_t) broker_port;

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(udp_port);
  if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    throw SystemError{"bind"};

  return sock;
}

ProxyInitializer::ProxyInitializer(int paramc, char* parameters[]) {
  if (!check_param_count(paramc))
    throw std::invalid_argument("Wrong number of arguments");

  read_params(paramc, parameters);
}

Proxy* ProxyInitializer::initialize_proxy() {
  size_t request_length;
  const char* request;
  int sock;
  struct http_helper helper;
  std::unique_ptr<Outputter> outputter;
  std::unique_ptr<Processor> processor;
  std::shared_ptr<ClientHandler> client_handler;

  sock = radio::NO_VALUE;

  std::string msg_str = build_request();

  request = msg_str.c_str();
  request_length = msg_str.length() + 1;

  try {
    sock = establish_connection();

    send_whole_message(sock, request, request_length);

    read_and_parse_response(sock, helper);

    if (!radio::is_set(broker_port)) {
      outputter = std::make_unique<StandardOutputter>();
    }
    else {
      int udp_sock = open_udp_socket();
      client_handler = std::make_shared<ClientHandler>(helper.name, udp_sock, broker_timeout);
      outputter = std::make_unique<UDPOutputter>(client_handler);
    }

    if (metadata && helper.metaint != radio::NO_VALUE) // Prosiliśmy o metadane i będziemy je dostawać.
      processor = std::make_unique<MetadataProcessor>(std::move(outputter), helper.metaint);
    else if (helper.metaint == radio::NO_VALUE) // Nie będziemy dostawać metadanych.
      processor = std::make_unique<NormalProcessor>(std::move(outputter));
    else // Nie prosiliśmy o metadane, a byśmy je dostawali.
      throw std::runtime_error("The radio has attempted to send metadata "
                               "even though it wasn't asked for it");

    if (!radio::is_set(broker_port))
      return new StandardProxy{sock, helper.buf_str, timeout, std::move(processor)};
    else
      return new UDPProxy{sock, helper.buf_str, timeout, std::move(processor), client_handler};
  }
  catch (const std::exception& e) {
    if (sock != radio::NO_VALUE)
      close(sock);

    throw;
  }
}