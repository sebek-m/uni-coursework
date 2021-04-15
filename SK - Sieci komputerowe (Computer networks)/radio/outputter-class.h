/*
  Klasa budująca fragment danych gotowy do bycia przekazanym dalej.
*/

#ifndef OUTPUTTER_CLASS_H
#define OUTPUTTER_CLASS_H

#include <cstddef>

#include "client-handler-class.h"

class Outputter {
public:
  Outputter() = default;
  virtual ~Outputter() = default;

  virtual void out(const char* data, size_t data_len) = 0;
  virtual void meta_out(const char* meta, size_t meta_len) = 0;
};

class StandardOutputter : public Outputter {
public:
  StandardOutputter() = default;

  void out(const char* data, size_t data_len) override;
  void meta_out(const char* meta, size_t meta_len) override;
};

class UDPOutputter : public Outputter {
private:
  const std::shared_ptr<ClientHandler> client_handler;

  void construct_and_send_msg(uint16_t type, const char* data, size_t data_len);
public:
  UDPOutputter(std::shared_ptr<ClientHandler> client_handler);

  void out(const char* data, size_t data_len) override;
  void meta_out(const char* meta, size_t meta_len) override;
};

#endif