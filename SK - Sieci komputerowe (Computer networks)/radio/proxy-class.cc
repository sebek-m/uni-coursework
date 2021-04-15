#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <iostream>

#include "proxy-class.h"
#include "exceptions.h"

namespace {
  const int RADIO_SOCK = 0;
  const int UDP_SOCK = 1;

  const int SOCKET_NUM = 2;
}

Proxy::Proxy(int sock, std::string initial_data, int radio_timeout,
             std::unique_ptr<Processor> processor) :
             sock(sock), read_bytes(initial_data.length()),
             radio_timeout(radio_timeout), processor(std::move(processor)) {
  strncpy(buf, initial_data.c_str(), initial_data.length());
}

Proxy::~Proxy() {
  if (close(sock))
    fprintf(stderr, "ERROR: cannot close sock\n");
}

StandardProxy::StandardProxy(int sock, std::string initial_data, int radio_timeout,
                             std::unique_ptr<Processor> processor) :
                             Proxy(sock, initial_data, radio_timeout, std::move(processor)) {}

UDPProxy::UDPProxy(int sock, std::string initial_data, int radio_timeout,
                   std::unique_ptr<Processor> processor,
                   std::shared_ptr<ClientHandler> client_handler) :
                   Proxy(sock, initial_data, radio_timeout, std::move(processor)),
                   client_handler(std::move(client_handler)) {}

void Proxy::stop() {
  STOP = true;
}

void StandardProxy::play() {
  // Podajemy dalej dane, które być może dostaliśmy w konstruktorze.
  if (read_bytes > 0)
    (*processor).process_and_out(read_bytes, buf);

  while (!STOP) {
    read_bytes = radio::read_and_handle(sock, buf, radio::BUF_SIZE);

    if (!STOP)
      (*processor).process_and_out(read_bytes, buf);
  }
}

void UDPProxy::play() {
  struct pollfd sockets[SOCKET_NUM];
  int ret;

  for (auto& poll_sock : sockets) {
    poll_sock.events = POLLIN;
    poll_sock.revents = 0;
  }

  sockets[RADIO_SOCK].fd = sock;
  sockets[UDP_SOCK].fd = (*client_handler).get_sock();

  // Podajemy dalej dane, które być może dostaliśmy w konstruktorze.
  if (read_bytes > 0)
    (*processor).process_and_out(read_bytes, buf);

  last_checkin = std::chrono::steady_clock::now();

  while (!STOP) {
    for (auto& poll_sock : sockets)
      poll_sock.revents = 0;

    ret = poll(sockets, SOCKET_NUM, radio_timeout * 1000);
    auto event_time = std::chrono::steady_clock::now();

    if (radio::timed_out(radio_timeout, event_time, last_checkin))
      throw std::runtime_error{"The radio has timed out"};

    if (ret == -1 && !radio::interrupted()) {
      throw std::runtime_error{"poll"};
    }
    else if (ret > 0) {
      if (!STOP && (sockets[RADIO_SOCK].revents & POLLIN)) {
        last_checkin = event_time;
        read_bytes = radio::read_and_handle(sock, buf, radio::BUF_SIZE);
      }

      if (!STOP && (sockets[UDP_SOCK].revents & (POLLIN | POLLERR)))
        (*client_handler).handle_client(event_time);

      if (!STOP && read_bytes > 0)
        (*processor).process_and_out(read_bytes, buf);
    }

    read_bytes = 0;
  }
}