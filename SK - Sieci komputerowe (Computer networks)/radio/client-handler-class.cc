#include <exception>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <utility>

#include "client-handler-class.h"
#include "client-info-class.h"
#include "utils.h"

ClientHandler::ClientHandler(const std::string& radio_name, int sock, int timeout)
                            : sock(sock), timeout(timeout) {
  radio::construct_msg(msg::IAM, iam_msg, radio_name);
}

ClientHandler::~ClientHandler() {
  if (close(sock))
    fprintf(stderr, "ERROR: cannot close sock\n");
}

auto ClientHandler::find_client(const struct sockaddr_in& client_address) {
  uint32_t address;
  in_port_t port;

  address = client_address.sin_addr.s_addr;
  port = client_address.sin_port;

  return clients.find({address, port});
}

void ClientHandler::maybe_delete_client(const struct sockaddr_in& client_address) {
  auto client_it = find_client(client_address);

  if (client_it != clients.end())
    clients.erase(client_it);
}

void ClientHandler::send_iam(const ClientInfo& client) {
  client.send(sock, iam_msg.data(), iam_msg.size());
}

const ClientInfo& ClientHandler::add_client(const struct sockaddr_in& client_address) {
  uint32_t address;
  in_port_t port;

  address = client_address.sin_addr.s_addr;
  port = client_address.sin_port;

  auto client_ptr = std::make_unique<ClientInfo>(client_address, event_time);
  const auto& ret = *client_ptr;

  clients.insert({{address, port}, std::move(client_ptr)});

  return ret;
}

void ClientHandler::handle_discover(const struct sockaddr_in& client_address) {
  auto client_it = find_client(client_address);

  if (client_it == clients.end()) {
    // Jest to pierwszy DISCOVER od takiego klienta.
    const auto& client = add_client(client_address);
    send_iam(client);
  }
  else {
    // Znamy już tego klienta.
    auto& client = *(client_it->second);

    if (client.is_alive(timeout, event_time) && !client.is_confirmed()) {
      client.confirm();
      client.update_time(event_time);
    }
    else {
      // Usuwamy jeśli minął timeout lub przysłał więcej niż dwa discover.
      clients.erase(client_it);
    }
  }
}

void ClientHandler::keep_alive(const struct sockaddr_in& client_address) {
  auto client_it = find_client(client_address);

  if (client_it == clients.end()) 
    return;

  auto& client = *(client_it->second);

  // Przyjmujemy KEEPALIVE nawet jeśli nie dostaliśmy jeszcze
  // drugiego DISCOVER bo być może odebraliśmy komunikaty
  // w innej kolejności.
  if (client.is_alive(timeout, event_time))
    client.update_time(event_time);
  else
    clients.erase(client_it);
}

void ClientHandler::interpret_msg(const struct sockaddr_in& client_address) {
  uint16_t type, length;

  type = ntohs(*(uint16_t*)buffer);
  length = ntohs(*(uint16_t*)(buffer + sizeof(type)));

  if (length != 0) {
    // Nieoczekiwana wartość pola length w nagłówku
    maybe_delete_client(client_address);
    return;
  }

  switch (type) {
    case msg::DISCOVER:
      handle_discover(client_address);
      break;
    case msg::KEEPALIVE:
      keep_alive(client_address);
      break;
    default:
      // Jeśli to klient, którego znamy wysłał
      // nam coś czego się nie spodziewaliśmy,
      // to go usuwamy.
      maybe_delete_client(client_address);
      break;
  }
}

void ClientHandler::handle_client(const radio::EventTime& poll_ev_time) {
  size_t read_bytes;
  socklen_t rcva_len;
  struct sockaddr_in client_address;

  event_time = poll_ev_time;

  rcva_len = (socklen_t) sizeof(client_address);
  read_bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                        (struct sockaddr*) &client_address, &rcva_len);

  // Nie chcemy, żeby klient wysyłał nam coś dłuższego niż sam nagłówek,
  // więc usuwamy go z clients jeśli tak zrobi i go znamy, ale jeśli dostaniemy coś
  // krótszego to tylko to ignorujemy, bo być może był to niepełny pakiet
  // z poprawnym komunikatem i istnieje szansa, że zaraz dostaniemy kolejny, już pełny.
  if (read_bytes == radio::HEADER_LENGTH)
    interpret_msg(client_address);
  else if (read_bytes > radio::HEADER_LENGTH && !radio::interrupted())
    maybe_delete_client(client_address);
}

void ClientHandler::send_to_all(const std::vector<char>& msg) {
  auto clients_it = clients.begin();

  while (clients_it != clients.end()) {
    auto& client = *(clients_it->second);

    if (!client.is_alive(timeout, event_time)) {
      // Klient przestał się do nas odzywać.
      clients_it = clients.erase(clients_it);
    }
    else {
      client.send(sock, msg.data(), msg.size());
      clients_it++;
    }
  }
}

int ClientHandler::get_sock() {
  return sock;
}