#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <vector>
#include <string>
#include <chrono>

/*
  Typy komunikatów.
*/
namespace msg {
  const uint16_t DISCOVER = 1;
  const uint16_t IAM = 2;
  const uint16_t KEEPALIVE = 3;
  const uint16_t AUDIO = 4;
  const uint16_t METADATA = 6; // W treści jest 6, pomimo tego, że jest 5 komunikatów.
}

/*
  Stałe i funkcje pomocnicze.
*/
namespace radio {
  const size_t BUF_SIZE = 1024;
  const size_t CLIENT_BUF_SIZE = 4096;

  const int DEFAULT_TIMEOUT = 5;
  const int NO_VALUE = -1;
  const std::string EMPTY_STRING = "";

  const size_t HEADER_LENGTH = 2 * sizeof(uint16_t);

  using EventTime = std::chrono::steady_clock::time_point;

  /*
    Czyta dane z gniazda i rozstrzyga, czy wystąpił błąd.
  */
  ssize_t read_and_handle(int sock, char* buf, size_t length);

  void construct_msg(uint16_t type, std::vector<char>& msg,
                   const char* data, size_t data_len);
  void construct_msg(uint16_t type, std::vector<char>& msg,
                    const std::string& data);

  /*
    Sprawdza, czy dana zmienna ma używalną wartość.
  */
  bool is_set(int var);
  bool is_set(const std::string& var);

  /*
    Sprawdza, czy między current a prev minęło więcej niż timeout sekund.
  */
  bool timed_out(int timeout, const radio::EventTime& current,
                const radio::EventTime& prev);

  /*
    Sprawdza, czy errno jest ustawione na EINTR.
  */
  bool interrupted();
}

namespace exit_code {
  const int SUCCESS = 0;
  const int ERROR = 1;
}

#endif