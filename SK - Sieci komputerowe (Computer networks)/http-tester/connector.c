#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"
#include "utils.h"
#include "connector.h"

#define BUFFER_SIZE 1024

/*
  Nawiązuje połączenie z podanym przy wywołaniu adresem i ustawia zmienną,
  na którą wskazuje sock, na deskryptor otwartego gniazdka.
*/
static int establish_connection(int* sock, arg_info* info) {
  int rc;
  struct addrinfo addr_hints;
  struct addrinfo *addr_result;

  *sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*sock < 0)
    return SOCKET_ERROR;

  memset(&addr_hints, 0, sizeof(struct addrinfo));
  addr_hints.ai_flags = 0;
  addr_hints.ai_family = AF_INET;
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_protocol = IPPROTO_TCP;

  rc =  getaddrinfo(info->connection_address, info->connection_port, &addr_hints, &addr_result);
  if (rc != 0)
    return GAI_ERROR_BASE + rc;

  if (connect(*sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0)
    return CONNECT_ERROR;

  freeaddrinfo(addr_result);

  return SUCCESS;
}

/*
  Wysyła message na dane gniazdko
*/
static int send_msg(int sock, size_t message_len, char* message) {
  ssize_t sent;
  size_t total_sent;

  total_sent = 0;

  while (total_sent < message_len) {
    sent = write(sock, message + total_sent, message_len - total_sent);
    if (sent < 0) {
      close (sock);
      return WRITE_ERROR;
    }

    total_sent += sent;
  }

  return SUCCESS;
}

/*
  Odbiera odpowiedź serwera i zapisuje ją w napisie wskazywanym przez response.
*/
static int receive_response(int sock, size_t* response_len, char** response) {
  size_t read_bytes, response_size;
  char buffer[BUFFER_SIZE];

  *response_len = 0;
  response_size = 1024;
  *response = (char*)malloc(BUFFER_SIZE);
  if (*response == NULL)
    return MEM_ERROR;

  while((read_bytes = read(sock, buffer, BUFFER_SIZE)) > 0) {
    if (*response_len + read_bytes >= response_size) {
      if (increase_mem(&response_size, response, read_bytes) < 0)
        return MEM_ERROR;
    }

    strncpy(*response + *response_len, buffer, read_bytes);
    *response_len += read_bytes;
  }

  if (read_bytes < 0) {
    free(*response);
    return READ_ERROR;
  }

  (*response)[*response_len] = '\0';

  return SUCCESS;
}

int send_and_receive_msg(arg_info* info, size_t message_len, char* message,
                         size_t* response_len, char** response) {
  int rc, sock;

  if ((rc = establish_connection(&sock, info)) != SUCCESS) {
    close(sock);
    return rc;
  }

  if ((rc = send_msg(sock, message_len, message)) != SUCCESS) {
    close(sock);
    return rc;
  }

  if ((rc = receive_response(sock, response_len, response)) != SUCCESS) {
    close(sock);
    return rc;
  }

  if (close(sock) < 0)
    return CLOSE_ERROR;

  return SUCCESS;
}