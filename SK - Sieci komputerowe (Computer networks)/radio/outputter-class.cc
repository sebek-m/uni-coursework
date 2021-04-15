#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <vector>

#include "outputter-class.h"
#include "client-handler-class.h"
#include "utils.h"

UDPOutputter::UDPOutputter(std::shared_ptr<ClientHandler> client_handler)
                          : client_handler(std::move(client_handler)) {}

void StandardOutputter::out(const char* data, size_t data_len) {
  fwrite(data, 1, data_len, stdout);
}

void StandardOutputter::meta_out(const char* meta, size_t meta_len) {
  fwrite(meta, 1, meta_len, stderr);
}

void UDPOutputter::construct_and_send_msg(uint16_t type, const char* data, size_t data_len) {
  std::vector<char> msg;

  radio::construct_msg(type, msg, data, data_len);
  (*client_handler).send_to_all(msg);
}

void UDPOutputter::out(const char* data, size_t data_len) {
  construct_and_send_msg(msg::AUDIO, data, data_len);
}

void UDPOutputter::meta_out(const char* data, size_t data_len) {
  construct_and_send_msg(msg::METADATA, data, data_len);
}