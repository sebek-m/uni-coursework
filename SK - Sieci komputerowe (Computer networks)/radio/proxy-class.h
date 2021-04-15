/*
  Główna klasa odbierająca dane z radia i kontrolująca ich dalszy los.
*/

#ifndef PROXY_CLASS_H
#define PROXY_CLASS_H

#include <string>
#include <memory>

#include "processor-class.h"
#include "client-handler-class.h"
#include "utils.h"

class Proxy {
protected:
  const int sock;

  ssize_t read_bytes;
  char buf[radio::BUF_SIZE];

  const int radio_timeout;
  radio::EventTime last_checkin;

  const std::unique_ptr<Processor> processor;

  /*
    Zmienna do przerwania pętli w metodzie play().
    Ustawiana na true w handlerze sygnału przerwania.
  */
  bool STOP = false;
public:
  Proxy(int sock, std::string initial_data, int radio_timeout,
        std::unique_ptr<Processor> processor);

  virtual ~Proxy();

  virtual void play() = 0;

  void stop();
};

class StandardProxy : public Proxy {
public:
  StandardProxy(int sock, std::string initial_data, int radio_timeout,
                std::unique_ptr<Processor> processor);

  void play() override;
};

class UDPProxy : public Proxy {
private:
  const std::shared_ptr<ClientHandler> client_handler;
public:
  UDPProxy(int sock, std::string initial_data, int radio_timeout,
           std::unique_ptr<Processor> processor,
           std::shared_ptr<ClientHandler> client_handler);
  
  void play() override;
};

#endif