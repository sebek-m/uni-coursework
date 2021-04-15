/*
  Klasa z informacjami o kliencie.
*/

#ifndef CLIENT_INFO_CLASS_H
#define CLIENT_INFO_CLASS_H

#include <cstddef>
#include <chrono>
#include <netinet/in.h>

#include "utils.h"

class ClientInfo {
private:
  struct sockaddr_in address;
  bool second_discover = false;

  /*
    Czas kontrolny, czyli kiedy klient dał ostatnio znać o swojej aktywności.
  */
  radio::EventTime last_checkin;
public:
  ClientInfo(const struct sockaddr_in& address, const radio::EventTime& connection_time);

  /*
    Wysyła do klienta wiadomość.
  */
  void send(int sock, const char* msg, size_t len) const;

  /*
    Sprawdza, czy klient jest aktywny,
    czyli czy od last_checkin nie minęło więcej niż timeout.
  */
  bool is_alive(int timeout, const radio::EventTime& current_time) const;

  /*
    Zwraca wartość second_discover.
  */
  bool is_confirmed() const;

  /*
    Ustawia second_discover na true.
  */
  void confirm();

  /*
    Ustawia last_checkin na current_time;
  */
  void update_time(const radio::EventTime& current_time);
};

#endif