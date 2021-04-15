/** @file
 * Implementacje funkcji przydatnych przy konwertowaniu jednych struktur/typów
 * w inne, by przechowywały podobne informacje w innej formie.
 *
 * @author Sebastian Miller
 * @date 18.05.2019
 */

#include "translators.h"
#include "helper-strucs.h"
#include "cmd-reader.h"

#include <stdlib.h>
#include <stdio.h>

unsigned stringToUnsigned(const char* string, size_t length, bool isNegative) {
    unsigned value, multiplier;
    size_t finish;

    value = 0;
    multiplier = 1;

    if (isNegative)
        finish = 1;
    else
        finish = 0;

    // "Odzyskujemy" liczby z charów reprezentujących cyfry
    for (size_t i = length; i > finish; multiplier *= 10, --i)
        value += (string[i - 1] - '0') * multiplier;

    return value;
}

int stringToInt(const char* string, size_t length) {
    unsigned uValue;
    int value;
    bool isNegative;

    isNegative = string[0] == '-';

    uValue = stringToUnsigned(string, length, isNegative);
    value = (int) uValue;

    if (isNegative)
        value = -value;

    return value;
}

void translateRoadDescription(CmdElement* city1El, CmdElement** city2El,
                              unsigned* length, int* year) {
    CmdElement *lengthEl, *yearEl;

    lengthEl = city1El->nextElement;
    yearEl = lengthEl->nextElement;
    *city2El = yearEl->nextElement;

    *length = stringToUnsigned(lengthEl->input,
                                 lengthEl->inputLength, false);
    *year = stringToInt(yearEl->input, yearEl->inputLength);
}

MaxNumber* prepareMaxNumber(unsigned maxValue) {
    MaxNumber* structure;
    size_t length;
    char* digits;
    unsigned maxNum, tmp;

    structure = (MaxNumber*)malloc(sizeof(MaxNumber));
    if (structure) {

        maxNum = maxValue;
        length = 0;

        // Zliczamy liczbę cyfr
        while (maxNum != 0) {
            maxNum /= 10;
            length++;
        }

        digits = (char*)malloc(length * sizeof(char));
        if (digits == NULL) {
            free(structure);
            return NULL;
        }

        maxNum = maxValue;

        // Poprzez dzielenie maksymalnej wartości w pętli przez 10 "kasujemy" kolejne
        // cyfry od końca, by móc je "sczytać" poprzez działanie modulo
        for (size_t i = length - 1; maxNum != 0; maxNum /= 10, i--) {
            tmp = maxNum % 10;
            // Kod ASCII to integer więc, po dodaniu od zmiennej tmp, która
            // reprezentuje cyfrę, kodu chara '0', równego 48,
            // dostaniemy kod ASCII chara, reprezentującego daną cyfrę.
            digits[i] = tmp + '0';
        }

        structure->digits = digits;
        structure->length = length;
    }

    return structure;
}