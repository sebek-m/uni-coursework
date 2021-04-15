#ifndef PROXY_INITIALIZER_H
#define PROXY_INITIALIZER_H

#include <string>

#include "utils.h"
#include "proxy-class.h"

namespace detail {
  const int DEFAULT_METADATA = false;
}

/*
  Klasa odpowiedzialna za przygotowanie pośrednika do pracy
  według podanych przy wywołaniu parametrów.
*/
class ProxyInitializer {
private:
  std::string host = radio::EMPTY_STRING;
  std::string resource = radio::EMPTY_STRING;
  std::string port = radio::EMPTY_STRING;
  int timeout = radio::DEFAULT_TIMEOUT;
  bool metadata = detail::DEFAULT_METADATA;

  int broker_port = radio::NO_VALUE;
  std::string multi_address = radio::EMPTY_STRING;
  int broker_timeout = radio::DEFAULT_TIMEOUT;

  /*
    Sprawdza czy podano wszystkie obowiązkowe parametry
    oraz czy przypadkiem nie podano opcjonalnych parametrów
    dla trybu pośrednika bez podawania parametru portu nasłuchiwania.
  */
  bool invalid_param_combo(bool broker_timeout_set);

  /*
    Przepisuje parametry do odpowiednich atrybutów, wcześniej sprawdzając
    czy są poprawne.
  */
  void read_params(int paramc, char* parameters[]);

  /*
    Tworzy odpowiednie żądanie HTTP do wysłania na serwer radia.
  */
  std::string build_request();

  /*
    Nawiązuje połączenie TCP z serwerem podanym w parametrze host.
    Zwraca deskryptor gniazda.
  */
  int establish_connection();

  /*
    Otwiera gniazdo UDP.
  */
  int open_udp_socket();
public:
  ProxyInitializer(int paramc, char* parameters[]);

  /*
    Tworzy wszystkie elementy potrzebne do działania pośrednika oraz
    tworzy instancję odpowiedniej podklasy klasy Proxy i zwraca na nią wskaźnik.
  */
  Proxy* initialize_proxy();
};

#endif