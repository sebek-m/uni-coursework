#include <sys/socket.h>

#include "client-info-class.h"
#include "exceptions.h"
#include "utils.h"

namespace {
  const socklen_t snda_len = (socklen_t) sizeof(struct sockaddr_in);
}

ClientInfo::ClientInfo(const struct sockaddr_in& address,
                       const radio::EventTime& connection_time)
                       : address(address), last_checkin(connection_time) {}

void ClientInfo::send(int sock, const char* msg, size_t len) const {
  ssize_t ret;

  ret = sendto(sock, msg, len, 0, (struct sockaddr*) &address, snda_len);
  if (ret < 0 && !radio::interrupted())
    throw SystemError{"sendto"};
}

bool ClientInfo::is_alive(int timeout, const radio::EventTime& current_time) const {
  return !radio::timed_out(timeout, current_time, last_checkin);
}

bool ClientInfo::is_confirmed() const {
  return second_discover;
}

void ClientInfo::confirm() {
  second_discover = true;
}

void ClientInfo::update_time(const radio::EventTime& current_time) {
  last_checkin = current_time;
}