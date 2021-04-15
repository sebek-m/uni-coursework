/*
  Klasa przetwarzająca dane, które przybyły z radia w odpowiedni sposób,
  zależny od tego czy spodziewamy się metadanych czy nie.
*/

#ifndef INTERPRETTER_CLASS_H
#define INTERPRETTER_CLASS_H

#include <string>
#include <memory>

#include "outputter-class.h"
#include "utils.h"

class Processor {
protected:
  const std::unique_ptr<Outputter> outputter;
public:
  Processor(std::unique_ptr<Outputter> outputter);
  virtual ~Processor() = default;

  virtual void process_and_out(size_t read_bytes, const char* buf) = 0;
};

class NormalProcessor : public Processor {
public:
  NormalProcessor(std::unique_ptr<Outputter> outputter);

  /*
    Przetwarza dane i odpowiednio przekazuje je dalej.
  */
  void process_and_out(size_t read_bytes, const char* buf) override;
};

class MetadataProcessor : public Processor {
private:
  const size_t metaint;

  /*
    Tryby przetwarzania danych w buforze.
  */
  enum class Mode {
    DATA,
    META_LENGTH,
    METADATA
  };

  /*
    Zmienne potrzebne do
    czytania danych przeplatanych z metadanymi.
  */
  Mode mode;
  size_t data_left, meta_length, meta_left;
  std::string meta_str;

  /*
    Metody pomocnicze służące do sczytywania i ewentualnego przekazywania dalej
    danych z aktualnego odcinka buf.
  */
  size_t process_data(size_t cur_pos, size_t read_bytes, const char* buf);
  size_t process_meta_length(size_t cur_pos, const char* buf);
  size_t process_metadata(size_t cur_pos, size_t read_bytes, const char* buf);  
public:
  MetadataProcessor(std::unique_ptr<Outputter> outputter, size_t metaint);
  
  void process_and_out(size_t read_bytes, const char* buf);
};

#endif