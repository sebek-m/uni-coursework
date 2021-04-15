#ifndef _ERR_
#define _ERR_

/*
  Makra informujące o rodzaju błędu, który miał miejsce lub o sukcesie wykonania funkcji.
*/
#define INVALID_RESPONSE -11
#define READ_ERROR -10
#define CLOSE_ERROR -9
#define WRITE_ERROR -8
#define CONNECT_ERROR -7
#define SOCKET_ERROR -6
#define FILE_CLOSE_ERROR -5
#define FILE_READ_ERROR -4
#define FILE_OPEN_ERROR -3
#define INVALID_ARGS_ERROR -2
#define MEM_ERROR -1
#define SUCCESS 0

/*
  Liczba dodawana do kodu błędu funkcji getaddresinfo, żeby nie pomylić go z którymś z powyższych kodów.
*/
#define GAI_ERROR_BASE 1000

/* Wypisuje informację o błędnym zakończeniu funkcji systemowej
i kończy działanie programu. */
extern void syserr(const char *fmt, ...);

/* Wypisuje informację o błędzie i kończy działanie programu. */
extern void fatal(const char *fmt, ...);

/* Wywołuje odpowiednią funkcję wypisującą błąd, w zależności od kodu błędu. */
extern void resolve_error(int err);

#endif
