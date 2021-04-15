/** @file
 * Struktury pomocnicze, przydatne przy czytaniu komend
 * oraz przy sprawdzaniu argumetnów komend.
 *
 * @author Sebastian Miller
 * @date 16.05.2019
 */

#ifndef DROGI_HELPER_STRUCS_H
#define DROGI_HELPER_STRUCS_H

#include <stdlib.h>

/**
 * Struktura przechowująca jeden element
 * polecenia z wejścia, czyli fragment od
 * średnika do średnika/od początku do średnika/
 * od średnika do końca.
 */
typedef struct CmdElement CmdElement;

/**
 * Struktura przechowująca napisową reprezentację
 * odpowiedniej maksymalnej wartości, np. typu unsigned int
 */
typedef struct MaxNumber MaxNumber;

/**
 * Struktura przechowująca jeden element
 * polecenia z wejścia, czyli fragment od
 * średnika do średnika/od początku do średnika/
 * od średnika do końca.
 */
struct CmdElement {
    char* input; /**< Wskaźnik na napis, będący fragmentem komendy */
    size_t inputLength; /**< Długość napisu */
    int mode; /**< Tryb czytania, czyli typ fragmentu (np. nazwa miasta) */
    CmdElement* nextElement; /**< Następny element polecenia w liście elementów,
                               * która reprezentuje linię wejścia */
};

/**
 * Struktura przechowująca napisową reprezentację
 * odpowiedniej maksymalnej wartości, np. typu unsigned int
 */
struct MaxNumber {
    char* digits; /**< Napis z cyframi liczby */
    size_t length; /**< Długość tego napisu */
};

#endif //DROGI_HELPER_STRUCS_H
