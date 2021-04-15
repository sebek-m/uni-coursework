#ifndef RADIO_CLIENT_CLASS_H
#define RADIO_CLIENT_CLASS_H

#include <string>
#include <vector>
#include <unordered_map>

#include "radio-info-class.h"

namespace detail {
  using RadioMap = std::unordered_map<std::string, std::unique_ptr<RadioInfo>>;
}

class RadioClient {
private:
  int proxy_sock = radio::NO_VALUE;
  int telnet_sock = radio::NO_VALUE;
  int rem_cont_sock = radio::NO_VALUE;

  std::string host = radio::EMPTY_STRING;
  int port = radio::NO_VALUE;
  int telnet_port = radio::NO_VALUE;

  int timeout = radio::DEFAULT_TIMEOUT;

  struct sockaddr_in search_addr;

  std::vector<char> discover_msg;
  std::vector<char> keepalive_msg;

  bool STOP = false;
  bool CONTROLLED = false;

  char buf[radio::HEADER_LENGTH + radio::CLIENT_BUF_SIZE];

  radio::EventTime event_time;

  size_t cur_option = 0;
  std::vector<std::string> menu_options;

  std::string metadata = radio::EMPTY_STRING;

  /*
    Mapa stacji radiowych wg ich nazwy.
  */
  detail::RadioMap radios;

  detail::RadioMap::iterator chosen_radio_it = radios.end();

  bool invalid_param_combo();

  void read_params(int paramc, char* parameters[]);

  void open_telnet_socket();
  void open_udp_socket();

  void del_menu_option(size_t pos);
  void update_radios();

  void show_menu();
  void show_menu(std::string metadata);

  void next();
  void prev();
  void run_menu_option();

  void handle_key_press();

  void maybe_add_option(const std::string& name);
  void handle_iam(const struct sockaddr_in& radio_address);
  void handle_audio(size_t length, const struct sockaddr_in& radio_address);
  void parse_meta();
  void handle_meta(const struct sockaddr_in& radio_address);

  void interpret_msg(size_t read_bytes, const struct sockaddr_in& radio_address);
  void handle_radio_msg();

  void inspect_chosen_radio();
public:
  RadioClient(int paramc, char* parameters[]);

  void start();
  void stop();
};

#endif