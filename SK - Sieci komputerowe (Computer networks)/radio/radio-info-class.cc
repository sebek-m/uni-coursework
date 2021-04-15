#include "radio-info-class.h"
#include "utils.h"

RadioInfo::RadioInfo(const std::string& name, const struct sockaddr_in& proxy_address)
                    : name(name), address(proxy_address) {}

bool RadioInfo::is_alive(int timeout, const radio::EventTime& cur_time) {
  return !radio::timed_out(timeout, cur_time, last_checkin);
}

void RadioInfo::update_time(const radio::EventTime& new_time) {
  last_checkin = new_time;
}

struct sockaddr_in RadioInfo::get_address() {
  return address;
}

bool RadioInfo::equals(const struct sockaddr_in& proxy_address) {
  return address.sin_addr.s_addr == proxy_address.sin_addr.s_addr &&
         address.sin_port == proxy_address.sin_port;
}