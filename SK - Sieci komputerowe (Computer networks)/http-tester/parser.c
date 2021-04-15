#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err.h"
#include "parser.h"
#include "utils.h"

#define INITIAL_STRING_SIZE 1024

/*
  Przepisuje po kolei znaki z source do destination, począwszy od start_pos, aż do napotkania któregoś znaku terminator.
  Zwraca błąd, jeśli nie napotkano znaku terminator.
*/
static int rewrite(size_t* pos, const char* source, char* destination, size_t start_pos, char terminator) {
  size_t i, j;

  for (i = start_pos, j = 0; source[i] != '\0' && source[i] != terminator; i++, j++)
    destination[j] = source[i];

  if (source[i] != terminator)
    return INVALID_ARGS_ERROR;

  destination[j] = '\0';

  *pos = i;

  return SUCCESS;
}

/*
  Znajduje pozycję, na której zaczyna się adres http, czyli jedna dalej niż koniec prefiksu http:// lub https://.
*/
static int find_http_address_start(size_t* start, const char* http_info) {
  char* colon;

  colon = strstr(http_info, "://");
  if (colon == NULL)
    return INVALID_ARGS_ERROR;

  *start = (colon + strlen("://")) - http_info;

  return SUCCESS;
}

/*
  Ekstrahuje z pierwszego argumentu wywołania adres połączenia i numer portu oraz umieszcza je odpowiednio w zmiennych
  connection_address i connection_port. Zwraca błąd, jeśli nie powiodła się alokacja pamięci lub argument okazał się niepoprawny.
*/
static int parse_connection_address(const char* connection_info,
                                    char** connection_address, char** connection_port) {
  size_t length, colon_pos, null_pos;

  length = strlen(connection_info);

  *connection_address = (char*)malloc(length);
  if (*connection_address == NULL)
    return MEM_ERROR;
  
  if (rewrite(&colon_pos, connection_info, *connection_address, 0, ':') != SUCCESS) {
    free(*connection_address);
    return INVALID_ARGS_ERROR;
  }

  *connection_port = (char*)malloc(length - colon_pos);
  if (*connection_port == NULL) {
    free(*connection_address);
    return MEM_ERROR;
  }

  rewrite(&null_pos, connection_info, *connection_port, colon_pos + 1, '\0');

  return SUCCESS;
}

/*
  Jeśli to konieczne, zwiększa rozmiar pamięci zaalokowanej na napis i dodaje do niego podany znak.
  Dodatkowo inkrementuje indeks, którego adresem jest i.
*/
static int add_char_to_string(char** array, char c, size_t* i, size_t* size) {
  if (*i == *size) {
    if (increase_mem(size, array, 1) < 0)
      return MEM_ERROR;
  }

  (*array)[*i] = c;
  (*i)++;

  return SUCCESS;
}

/*
  Przepisuje ciasteczka z pliku do napisu,
  który zostanie później umieszczony w żadaniu HTTP.
*/
static int rewrite_cookies(FILE* cookies_file, char** cookies, size_t* cookies_len) {
  char c, next;
  size_t i, cookies_string_size;

  i = 0;
  c = '\0';

  cookies_string_size = INITIAL_STRING_SIZE;
  *cookies = (char*)malloc(INITIAL_STRING_SIZE);
  if (*cookies == NULL)
    return MEM_ERROR;

  while ((c = fgetc(cookies_file)) != EOF) {
    if (c != '\n' && c != '\r' && c != EOF) {
      if (add_char_to_string(cookies, c, &i, &cookies_string_size) != SUCCESS)
        return MEM_ERROR;
    }
    else if (c == '\n') {
      // Podglądamy, czy po znaku nowej linii są jeszcze jakieś ciasteczka
      // i jeśli tak to dołączamy "; " do napisu.
      if ((next = fgetc(cookies_file)) != EOF) {
        if (ungetc(next, cookies_file) == EOF) {
          free(*cookies);
          return FILE_READ_ERROR;
        }
        
        if (add_char_to_string(cookies, ';', &i, &cookies_string_size) != SUCCESS ||
            add_char_to_string(cookies, ' ', &i, &cookies_string_size) != SUCCESS)
          return MEM_ERROR;
      }
    }
  }

  if (ferror(cookies_file))
    return FILE_READ_ERROR;

  if (add_char_to_string(cookies, '\0', &i, &cookies_string_size) != SUCCESS)
    return MEM_ERROR;

  *cookies_len = i - 1;

  return SUCCESS;
}

/*
  Otwiera plik i wywołuje funkcję, która przepisuje ciasteczka do napisu.
*/
static int read_cookies(const char* file_name, size_t* cookies_len, char** cookies) {
  FILE* cookies_file;
  int rc;

  if (!(cookies_file = fopen(file_name, "r")))
    return FILE_OPEN_ERROR;

  if ((rc = rewrite_cookies(cookies_file, cookies, cookies_len)) < 0) {
    fclose(cookies_file);
    return rc;
  }

  if (fclose(cookies_file) < 0)
    return FILE_CLOSE_ERROR;

  return SUCCESS;
}

/*
  Ekstrahuje z trzeciego argumentu wywołania adres http z ewentualnym numerem portu i adres zasobu http
  oraz umieszcza je odpowiednio w zmiennych http_address i http_resource.
  Zwraca błąd, jeśli nie powiodła się alokacja pamięci.
*/
static int parse_http_address(const char* http_info, size_t* http_addr_len, char** http_address,
                              size_t* http_rsc_len, char** http_resource) {
  size_t length, http_start, resource_start, null_pos;

  length = strlen(http_info);

  if (find_http_address_start(&http_start, http_info) != SUCCESS)
    return INVALID_ARGS_ERROR;

  *http_address = (char*)malloc(length - http_start + 1);
  if (*http_address == NULL)
    return MEM_ERROR;

  if (rewrite(&resource_start, http_info, *http_address, http_start, '/') != SUCCESS) {
    free(*http_address);
    return INVALID_ARGS_ERROR;
  }

  *http_addr_len = resource_start - http_start;

  *http_resource = (char*)malloc(length - resource_start + 1);
  if (*http_resource == NULL) {
    free(*http_address);
    return MEM_ERROR;
  }

  rewrite(&null_pos, http_info, *http_resource, resource_start, '\0');

  *http_rsc_len = length - resource_start;

  return SUCCESS;
}

void initialize(arg_info* info) {
  info->connection_port = NULL;
  info->connection_address = NULL;
  info->cookies = NULL;
  info->http_address = NULL;
  info->http_resource = NULL;

  info->cookies_len = 0;
}

int read_cmd (arg_info* info, int argc, char* argv[]) {
  int ret;
  size_t cookies_len, http_addr_len, http_rsc_len;
  char *connection_port, *connection_address, *cookies, *http_address, *http_resource;

  if (argc != 4)
    fatal("Usage: %s connection_address:port cookie_file http_server", argv[0]);

  if ((ret = parse_connection_address(argv[1], &connection_address,
                                      &connection_port)) != SUCCESS) {
    return ret;
  }

  if ((ret = read_cookies(argv[2], &cookies_len, &cookies)) != SUCCESS) {
    free(connection_address);
    return ret;
  }

  if ((ret = parse_http_address(argv[3], &http_addr_len, &http_address,
                                &http_rsc_len, &http_resource)) != SUCCESS) {
    free(connection_address);
    free(cookies);
    return ret;
  }

  info->connection_port = connection_port;
  info->connection_address = connection_address;
  info->cookies_len = cookies_len;
  info->cookies = cookies;
  info->http_addr_len = http_addr_len;
  info->http_address = http_address;
  info->http_rsc_len = http_rsc_len;
  info->http_resource = http_resource;
  
  return SUCCESS;
}

void clear_info(arg_info* info) {
  if (info->connection_port != NULL) {
    free(info->connection_port);
    info->connection_port = NULL;
  }

  if (info->connection_address != NULL) {
    free(info->connection_address);
    info->connection_address = NULL;
  }

  if (info->cookies != NULL) {
    free(info->cookies);
    info->cookies = NULL;
  }

  if (info->http_address != NULL) {
    free(info->http_address);
    info->http_address = NULL;
  }

  if (info->http_resource != NULL) {
    free(info->http_resource);
    info->http_resource = NULL;
  }

  info->cookies_len = 0;
  info->http_addr_len = 0;
  info->http_rsc_len = 0;
}