#include "processor-class.h"
#include "outputter-class.h"
#include "utils.h"

namespace {
  /*
    Mnożnik bajta długości metadanych.
  */
  const size_t META_LEN_MUL = 16;
}

Processor::Processor(std::unique_ptr<Outputter> outputter)
: outputter(std::move(outputter)) {}

NormalProcessor::NormalProcessor(std::unique_ptr<Outputter> outputter)
: Processor(std::move(outputter)) {}

MetadataProcessor::MetadataProcessor(std::unique_ptr<Outputter> outputter, size_t metaint)
: Processor(std::move(outputter)), metaint(metaint), mode(Mode::DATA), data_left(metaint) {}

void NormalProcessor::process_and_out(size_t read_bytes, const char* buf) {
  (*outputter).out(buf, read_bytes);
}

size_t MetadataProcessor::process_data(size_t cur_pos, size_t read_bytes, const char* buf) {
  size_t received_data;

  received_data = std::min<size_t>(data_left, read_bytes - cur_pos);
  data_left -= received_data;

  (*outputter).out(buf + cur_pos, received_data);

  if (data_left == 0) {
    mode = Mode::META_LENGTH;
    data_left = metaint;
  }

  return received_data;
}

size_t MetadataProcessor::process_meta_length(size_t cur_pos, const char* buf) {
  meta_length = META_LEN_MUL * (*(uint8_t*)(buf + cur_pos));
  meta_left = meta_length;

  mode = meta_length > 0 ? Mode::METADATA : Mode::DATA;

  return sizeof(char);
}

size_t MetadataProcessor::process_metadata(size_t cur_pos, size_t read_bytes, const char* buf) {
  size_t received_meta;

  received_meta = std::min<size_t>(meta_left, read_bytes - cur_pos);
  meta_left -= received_meta;

  // Być może nie otrzymaliśmy jeszcze wszystkich zapowiedzianych
  // metadanych, więc zachowujemy je na potem.
  meta_str.append(buf + cur_pos, received_meta);

  if (meta_str.length() == meta_length) {
    (*outputter).meta_out(meta_str.c_str(), meta_str.length());
    meta_str.clear();

    mode = Mode::DATA;
  }

  return received_meta;
}

void MetadataProcessor::process_and_out(size_t read_bytes, const char* buf) {
  size_t cur_pos;

  cur_pos = 0;

  while (cur_pos < read_bytes) {
    switch (mode) {
      case Mode::DATA:
        cur_pos += process_data(cur_pos, read_bytes, buf);
        break;
      case Mode::META_LENGTH:
        cur_pos += process_meta_length(cur_pos, buf);
        break;
      case Mode::METADATA:
        cur_pos += process_metadata(cur_pos, read_bytes, buf);
        break;
      default:
        break;
    }
  }
}
