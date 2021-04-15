#ifndef RADIO_INFO_CLASS_H
#define RADIO_INFO_CLASS_H

#include <netinet/in.h>

#include "utils.h"

class RadioInfo {
private:
  std::string name;
  struct sockaddr_in address;

  radio::EventTime last_checkin;
public:
  RadioInfo(const std::string& name, const struct sockaddr_in& proxy_address);

  bool is_alive(int timeout, const radio::EventTime& cur_time);
  void update_time(const radio::EventTime& new_time);
  struct sockaddr_in get_address();
  bool equals(const struct sockaddr_in& prpoxy_address);
};

#endif