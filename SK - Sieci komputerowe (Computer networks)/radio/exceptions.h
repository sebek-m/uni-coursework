#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <netdb.h>
#include <exception>
#include <string>

/*
  Wyjątek rzucany w przypadku napotkania błędu funkcji systemowej.
*/
class SystemError : public std::exception {
private:
  std::string msg;
public:
  explicit SystemError(const std::string& msg) : msg(msg) {}

  const char * what() const noexcept override {
    return msg.c_str();
  }
};

/*
  Wyjątek rzucany w przypadku napotkania błędu funkcji getaddrinfo.
*/
class GAIError : public SystemError {
public:
  explicit GAIError(int rc) : SystemError(std::string{"getaddrinfo: "} + gai_strerror(rc)) {}
};

#endif
