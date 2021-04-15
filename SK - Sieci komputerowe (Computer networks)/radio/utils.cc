#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <stdexcept>

#include "utils.h"
#include "exceptions.h"

ssize_t radio::read_and_handle(int sock, char* buf, size_t length) {
  ssize_t read_bytes;

  read_bytes = read(sock, buf, length);
  if (read_bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
    throw std::runtime_error{"The radio has timed out"};
  if (read_bytes < 0 && !radio::interrupted())
    throw SystemError{"read"};
  else if (read_bytes == 0)
    throw std::runtime_error{"The radio has closed the connection"};

  return read_bytes;
}

void radio::construct_msg(uint16_t type, std::vector<char>& msg,
                   const char* data, size_t data_len) {
  uint16_t type_net, length_net;

  msg.resize(radio::HEADER_LENGTH + data_len);

  type_net = htons(type);
  length_net = htons(data_len);

  memcpy(msg.data(), (char*) &type_net, sizeof(type_net));
  memcpy(msg.data() + sizeof(type_net), (char*) &length_net, sizeof(length_net));

  if (data_len > 0)
    memcpy(msg.data() + radio::HEADER_LENGTH, data, data_len);
}

void radio::construct_msg(uint16_t type, std::vector<char>& msg,
                   const std::string& data) {
  radio::construct_msg(type, msg, data.c_str(), data.length());
}

bool radio::is_set(int var) {
  return var != radio::NO_VALUE;
}

bool radio::is_set(const std::string& var) {
  return var != "";
}

bool radio::timed_out(int timeout, const radio::EventTime& current,
               const radio::EventTime& prev) {
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(current - prev);

  return duration.count() >= timeout;
}

bool radio::interrupted() {
  return errno == EINTR;
}