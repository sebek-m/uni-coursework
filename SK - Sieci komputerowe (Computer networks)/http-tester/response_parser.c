#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include <stdio.h>

#include "err.h"
#include "response_parser.h"
#include "utils.h"

#define BUFFER_SIZE 1024

/*
  Wpisuje do output kod statusu, o którym wcześniej się przekonano, że jest inny niż 200.
*/
static int obtain_status_code(char** output, char* current_line) {
  size_t http_ver_len, code_len;
  
  http_ver_len = strlen("HTTP/1.1 ");
  code_len = strlen(current_line) - http_ver_len;

  *output = (char*)malloc(code_len + 1);
  if (*output == NULL)
    return MEM_ERROR;

  strncpy(*output, current_line + http_ver_len, code_len);
  (*output)[code_len] = '\0';

  return SUCCESS;
}

/*
  Przekształca napis zakończony nullem w jego wersję napisaną małymi literami.
*/
static void str_to_lower(char* str) {
  for (size_t i = 0; str[i] != '\0'; i++)
    str[i] = tolower(str[i]);
}

/*
  Zwraca długość nazwy nagłówka z opcjonalnym białym znakiem.
*/
static size_t resolve_header_length(char* line_copy, const char* header_base) {
  size_t header_len;

  header_len = strlen(header_base);

  if (line_copy[header_len] == ' ' || line_copy[header_len] == '\t')
    header_len++;

  return header_len;
}

/*
  Odczytuje ciasteczko z linii, o której wiadomo, że zawiera header set-cookie,
  i wraz ze znakiem nowej linii dopisuje je do output.
*/
static int read_and_append_cookie(char** output, size_t* output_len, size_t* output_size,
                                  char* current_line, char* line_copy) {
  size_t header_len, cookie_len;
  char *cookie_start, *semicolon, *whitespace;

  header_len = resolve_header_length(line_copy, "set-cookie:");

  cookie_start  = current_line + header_len;

  semicolon = strchr(cookie_start, ';');
  // Sprawdzamy, czy w linii znajduje się coś więcej poza pierwszym ciasteczkiem.
  if (semicolon != NULL) {
    *semicolon = '\0';
  }
  else {
    // Sprawdzamy, czy jest biały znak przed CRLF.
    whitespace = strstr(cookie_start, " \r\n");
    if (whitespace == NULL)
      whitespace = strstr(cookie_start, "\t\r\n");

    if (whitespace != NULL)
      *whitespace = '\0';
  }

  cookie_len = strlen(cookie_start);
  if (*output_len + cookie_len >= *output_size) {
    // + 1 na znak nowej linii
    if (increase_mem(output_size, output, cookie_len + 1) < 0)
      return MEM_ERROR;
  }

  strcpy(*output + *output_len, cookie_start);
  *output_len += cookie_len + 1;
  (*output)[*output_len - 1] = '\n';

  return SUCCESS;
}

/*
  Czyta wartość zapisaną dla nagłówka content-length.
*/
static int read_content_length(char* current_line, char* line_copy,
                               bool* content_length_present, size_t* expected_content_length) {
  size_t header_len, tmp_content_length;

  header_len = resolve_header_length(line_copy, "content-length:");

  if (*content_length_present)
    tmp_content_length = *expected_content_length;

  if (sscanf(current_line + header_len, "%zu", expected_content_length) != 1)
    return INVALID_RESPONSE;

  // Sprawdzamy, czy w przypadku wielu pól content-length wszystkie mają taką samą wartość.
  // (RFC 7230, section 3.3.2)
  if (*content_length_present && *expected_content_length != tmp_content_length)
    return INVALID_RESPONSE;

  *content_length_present = true;

  return SUCCESS;
}

/*
  Czyta rozmiar chunka przy załoeżeniu, że current_line jest ustawione na początek linii z tym rozmiarem.
*/
static int read_chunk_size(char** cur_line_end, char* current_line, size_t* chunk_size) {
  char* semicolon;

  *cur_line_end = strstr(current_line, "\r\n");
  if (*cur_line_end == NULL)
    return INVALID_RESPONSE;

  **cur_line_end = '\0';

  // Znajdujemy początek ewentualnych chunk-extensions
  semicolon = strchr(current_line, ';');
  if (semicolon != NULL)
    *semicolon = '\0';

  if (sscanf(current_line, "%zX", chunk_size) != 1)
    return INVALID_RESPONSE;

  return SUCCESS;
}

/*
  Oblicza długość zasobu w sposób zależny od tego, czy ustawiono transfer-encoding: chunked.
*/
static int read_resource_length(bool is_chunked, size_t* real_content_len, size_t expected_content_len,
                                char** current_line, char** cur_line_end) {
  if (!is_chunked) {
    *real_content_len = strlen(*current_line);
  }
  else {
    char* chunk;
    size_t chunk_size;

    *real_content_len = 0;
    
    if (read_chunk_size(cur_line_end, *current_line, &chunk_size) < 0)
      return INVALID_RESPONSE;

    while (chunk_size > 0) {
      chunk = *cur_line_end + strlen("\r\n");

      size_t i;
      for (i = 0; i < chunk_size && chunk[i] != '\0'; i++);
      if (i < chunk_size)
        return INVALID_RESPONSE;

      *real_content_len += chunk_size;

      *current_line = chunk + chunk_size + strlen("\r\n");

      if (read_chunk_size(cur_line_end, *current_line, &chunk_size) < 0)
        return INVALID_RESPONSE;
    }
  }

  if (*real_content_len < expected_content_len)
    return INVALID_RESPONSE;

  return SUCCESS;
}

int parse_response(char* response, char** output) {
  char *headers_end, *current_line, *cur_line_end, *status_code;
  size_t expected_content_length, real_content_len, length_len, output_len, output_size;
  bool is_chunked, content_length_present;
  char length_buffer[BUFFER_SIZE];
  int rc;

  is_chunked = false;
  content_length_present = false;
  expected_content_length = 0;

  // Szukamy końca ostatniej linii nagłówków (lub linii statusu w przypadku braku nagłówków).
  headers_end = strstr(response, "\r\n\r\n");
  if (headers_end == NULL)
    return INVALID_RESPONSE;

  current_line = response;

  cur_line_end = strstr(current_line, "\r\n");
  if (cur_line_end == NULL || strstr(current_line, "HTTP/1.1 ") == NULL)
    return INVALID_RESPONSE;

  *cur_line_end = '\0';

  status_code = strstr(current_line, "200 OK");
  // Jeśli nie ma kodu 200 OK, to znaczy, że jest inny i chcemy go umieścić w raporcie.
  if (status_code == NULL)
    return obtain_status_code(output, current_line);

  // Przesuwamy się za "\r\n".
  current_line = cur_line_end + strlen("\r\n");

  output_len = 0;
  output_size = BUFFER_SIZE;
  *output = (char*)malloc(BUFFER_SIZE);
  if (*output == NULL)
    return MEM_ERROR;

  // Czytanie i interpretowanie headerów.
  while (cur_line_end != headers_end) {
    cur_line_end = strstr(current_line, "\r\n");
    if (cur_line_end == NULL) {
      free(*output);
      return INVALID_RESPONSE;
    }

    *cur_line_end = '\0';

    // Będziemy szukać nazw headerów w wersji lowercase linii, ponieważ wielkość znaków nie ma znaczenia
    // w komunikacie HTTP. (RFC 7230, section 3.2)
    char line_copy[strlen(current_line) + 1];
    strcpy(line_copy, current_line);
    str_to_lower(line_copy);

    // Sprawdzamy czy na pewno nazwa headera jest na początku linii, jednocześnie przekonując się o tym,
    // czy właśnie taki header jest w tej linii.
    if (strstr(line_copy, "set-cookie:") == line_copy) {
      if ((rc = read_and_append_cookie(output, &output_len, &output_size,
                                       current_line, line_copy)) < 0) {
        return rc;
      }
    }
    else if (strstr(line_copy, "content-length:") == line_copy) {
      if ((rc = read_content_length(current_line, line_copy, &content_length_present,
                                    &expected_content_length)) < 0) {
        free(*output);
        return rc;
      }
    }
    else if (!is_chunked && strstr(line_copy, "transfer-encoding:") == line_copy) {
      is_chunked = strstr(line_copy, "chunked") != NULL;
    }

    current_line = cur_line_end + strlen("\r\n");
  }

  // Przeskakujemy pustą linię.
  current_line = current_line + strlen("\r\n");

  if (read_resource_length(is_chunked, &real_content_len, expected_content_length,
                           &current_line, &cur_line_end) < 0) {
    free(*output);
    return INVALID_RESPONSE;
  }

  sprintf(length_buffer, "Dlugosc zasobu: %zu", real_content_len);

  length_len = strlen(length_buffer);
  if (output_len + length_len >= output_size) {
    if (increase_mem(&output_size, output, length_len + 1) < 0)
      return MEM_ERROR;
  }

  strcpy(*output + output_len, length_buffer);

  return SUCCESS;
}