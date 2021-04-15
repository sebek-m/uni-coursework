#ifndef _RESPONSE_PARSER_
#define _RESPONSE_PARSER_

/*
  Odczytuje odpowiednie informacje z napisu z odpowiedzią serwera, tworzy raport i zapisuje go w output.
*/
int parse_response(char* response, char** output);

#endif