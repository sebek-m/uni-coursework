/** @file
 * Funkcje potrzebne do implementacji funkcji getRouteDescription.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "route-description.h"
#include "utils.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Początkowa liczba cyfr w stringu reprezentującym liczbę
 */
#define INITIAL_DIGIT_NUM 6
/**
 * Początkowy rozmiar napisu
 */
#define INITIAL_STRING_SIZE 100

char const* emptyString() {
    return (char const*)calloc(1, sizeof(char));
}

/** @brief Odwraca napis.
 * Odwraca napis.
 *
 * @param[in] string       - odwracany napis;
 * @param[in] length       - długość odwracanego napisu;
 */
static void reverseString(char* string, size_t length) {
    char tmp;
    size_t j = length - 1;

    for (size_t i = 0; i < j; i++, j--) {
        tmp = string[j];
        string[j] = string[i];
        string[i] = tmp;
    }
}

/** @brief Przekształca liczbę całkowitą w napis.
 * Przekształca liczbę całkowitą w napis, alokując przy tym an niego pamięć.
 * Dodatkowo ustala rozmiar tego napisu.
 *
 * @param[in] number        - liczba, którą chcemy przekształcić;
 * @param[in,out] length    - wskaźnik na długość napisu reprezentującego liczbę;
 * @return Wskaźnik na powstały napis reprezentujący liczbę lub NULL, gdy jakaś
 * alokacja się nie powiodła.
 */
static char* intToString(long int number, size_t* length) {
    bool negative, noError;
    char* string;
    size_t size, i;

    string = (char*)malloc(INITIAL_DIGIT_NUM * sizeof(char));
    if (string == NULL)
        return NULL;

    size = INITIAL_DIGIT_NUM;
    negative = false;
    noError = true;

    if (number < 0) {
        negative = true;
        number = -number;
    }

    for (i = 0; number != 0 && noError; i++) {
        if (i == size) {
            size = 1 + size * 3 / 2;
            string = (char*)realloc(string, size * sizeof(char));
            if (string == NULL)
                noError = false;
        }

        if (noError) {
            long int remainder = number % 10;
            string[i] = remainder + '0';
            number = number / 10;
        }
    }

    if (!noError)
        return NULL;

    if (i == size) {
        // Potrzebne tylko na dopisanie ewentualnego minusa
        // i znaku końca napisu.
        size += 3;
        string = (char*)realloc(string, size * sizeof(char));
        if (string == NULL)
            return NULL;
    }

    if (negative) {
        string[i] = '-';
        i++;
    }

    string[i] = '\0';
    *length = i;

    reverseString(string, i);

    return string;
}

/** @brief Dodaje liczbę do napisu.
 * Dodaje napisową reprezentację liczby na koniec danego napisu.
 * Jesli nie starczy na nią miejsca, alokuje dodatkową pamięć.
 *
 * @param[in,out] string    - napis, do którego dołączamy liczbę;
 * @param[in] number        - liczba, którą dołączamy;
 * @param[in,out] pos       - wskaźnik na indeks ostatniego znaku w napisie;
 * @param[in,out] size      - wskaźnik na rozmiar tablicy reprezentującej napis;
 * @return Wartość @p true, jeśli udało się dodać napis, @p false jeśli alokacja
 * pamięci sie nie powiodła.
 */
static bool addNumber(char** string, long int number, size_t* pos, size_t* size) {
    char *tmp;
    size_t tmpLength;

    tmp = intToString(number, &tmpLength);
    if (tmp == NULL) {
        free(*string);
        return false;
    }

    // Dodatkowe 2 na późniejsze dodanie średnika
    if (*pos + tmpLength + 2 >= *size) {
        *size = tmpLength + (*size) * 3 / 2;
        *string = (char*)realloc(*string, (*size) * sizeof(char));
        if (*string == NULL) {
            free(tmp);
            return false;
        }
    }
    strcat(*string, tmp);
    free(tmp);
    strcat(*string, ";");
    *pos += tmpLength + 1;

    return true;
}

/** @brief Dodaje nazwę miasta do napisu.
 * Dodaje napis, będący nazwą miasta na koniec danego napisu.
 * Jesli nie starczy na niego miejsca, alokuje dodatkową pamięć.
 *
 * @param[in,out] string    - napis, do którego dołączamy nazwę;
 * @param[in] city          - miasto, którego nazwę dołączamy;
 * @param[in,out] pos       - wskaźnik na indeks ostatniego znaku w napisie;
 * @param[in,out] size      - wskaźnik na rozmiar tablicy reprezentującej napis;
 * @param[in] control       - wskaźnik informujący o tym, czy jesteśmy na końcu
 *                            opisu drogi, czy nie;
 * @return Wartość @p true, jeśli udało się dodać napis, @p false jeśli alokacja
 * pamięci sie nie powiodła.
 */
static bool addName(char** string, City* city, size_t* pos, size_t* size, Route* control) {
    size_t tmpLength = strlen(city->name);

    // Dodatkowe 2 na późniejsze dodanie średnika
    if (*pos + tmpLength + 2 >= *size) {
        *size = tmpLength + (*size) * 3 / 2;
        *string = (char*)realloc(*string, (*size) * sizeof(char));
        if (*string == NULL)
            return false;
    }
    strcat(*string, city->name);
    if (control != NULL)
        strcat(*string, ";");
    *pos += tmpLength + 1;

    return true;
}

char const* toString(WholeRoute* route) {
    char *string;
    size_t size, pos;
    Route* current;
    City* previousCity;

    string = (char*)calloc(INITIAL_STRING_SIZE, sizeof(char));
    if (string == NULL)
        return NULL;

    size = INITIAL_STRING_SIZE;
    pos = 0;
    current = route->first;

    // id drogi
    if (!addNumber(&string, route->routeId, &pos, &size))
        return NULL;

    // Miasto na początku drogi.
    if (!addName(&string, route->city1, &pos, &size, current))
        return NULL;
    previousCity = route->city1;

    // Informacje od odcinkach drogi i kolejnych miastach
    while (current != NULL) {
        if (!addNumber(&string, (current->roadSeg)->length,
                       &pos, &size))
            return NULL;

        if (!addNumber(&string, (current->roadSeg)->lastRepairYear,
                       &pos, &size))
            return NULL;

        City* nextCity = otherCity(current->roadSeg, previousCity);
        if (!addName(&string, nextCity, &pos, &size, current->nextSegment))
            return NULL;
        previousCity = nextCity;

        current = current->nextSegment;
    }

    return string;
}