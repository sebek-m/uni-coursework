#ifndef CLIENT_HANDLER_CLASS_H
#define CLIENT_HANDLER_CLASS_H

#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <netinet/in.h>
#include <chrono>

#include "client-info-class.h"
#include "utils.h"

namespace detail {
  struct pair_hash {
    /*
      Funkcja hashująca dla pary nieujemnych liczb całkowitych.
      Potrzebna, by użyć takiej pary jako klucza do mapy w ClientHandler.
    */
    size_t operator()(const std::pair<uint32_t, in_port_t>& client_id) const {
      // http://szudzik.com/ElegantPairing.pdf
      size_t x = client_id.first;
      size_t y = client_id.second;
      return x >= y ? (x * x + x + y) : (y * y + x);
    }
  };
}

/*
  Klasa przechowująca informacje o klientach, która także zarządza ich obsługą,
  czyli wymianą komunikatów.
*/
class ClientHandler {
private:
  std::vector<char> iam_msg;
  const int sock;
  const int timeout;

  char buffer[radio::BUF_SIZE];

  /*
    Czas zdarzenia na gnieździe UDP, czyli moment przysłania przez
    któregoś z klientów jakiegoś komunikatu.
  */
  radio::EventTime event_time;

  /*
    Mapa klientów wg ich adresu w postaci liczbowej i portu.
  */
  std::unordered_map<std::pair<uint32_t, in_port_t>, std::unique_ptr<ClientInfo>, detail::pair_hash> clients;

  /*
    Wysyła do klienta komunikat typu IAM.
  */
  void send_iam(const ClientInfo& client);

  /*
    Dodaje do clients klienta opisanego w client_address.
  */
  const ClientInfo& add_client(const struct sockaddr_in& client_address);

  /*
    Jeśli opisany w client_address klient jest w clients, to go usuwa.
  */
  void maybe_delete_client(const struct sockaddr_in& client_address);

  /*
    Zwraca iterator na element mapy z klientem opisanym w client_address
    lub iterator końcowy, jeśli nie ma takiego klienta.
  */
  auto find_client(const struct sockaddr_in& client_address);

  /*
    Obsługuje odebrany komunikat DISCOVER.
  */
  void handle_discover(const struct sockaddr_in& client_address);

  /*
    Sprawdza, czy klient o podanym adresie jest aktywny i, jeśli tak,
    uaktualnia jego czas kontrolny. W przeciwnym wypadku usuwa go z clients.
  */
  void keep_alive(const struct sockaddr_in& client_address);

  /*
    Rozpoznaje odebraną wiadomość i podejmuje odpowiednie kroki.
  */
  void interpret_msg(const struct sockaddr_in& client_address);
public:
  ClientHandler(const std::string& radio_name, int sock, int timeout);

  ~ClientHandler();

  /*
    Odbiera wiadomość, o której przybyciu zasygnalizował wcześniej poll
    oraz wywołuje interpret_msg.
  */
  void handle_client(const radio::EventTime& poll_ev_time);

  /*
    Wysyła wiadomość w msg do wszystkich zapisanych, aktywnych klientów.
    Przy okazji usuwa nieaktywnych klientów z clients.
  */
  void send_to_all(const std::vector<char>& msg);

  int get_sock();
};


#endif