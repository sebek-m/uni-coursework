/** @file
 * Funkcje potrzebne przy konwertowaniu jednych struktur/typów
 * w inne, by przechowywały podobne informacje w innej formie.
 *
 * @author Sebastian Miller
 * @date 18.05.2019
 */

#ifndef DROGI_TRANSLATORS_H
#define DROGI_TRANSLATORS_H

#include "map.h"
#include "helper-strucs.h"
#include "cmd-reader.h"

#include <stdlib.h>

/** @brief Odzyskuje odpowiednie informacje z opisu odcinka drogi.
 * Odzyskuje odpowiednie informacje z opisu odcinka drogi, podczas
 * tworzenia na podstawie tego opisu drogi krajowej.
 *
 * @param city1            - wskaźnik na element komendy z nazwą pierwszego miasta
 *                           podłączonego do odcinka;
 * @param city2El          - wskaźnik na wskaźnik na element komendy z nazwą drugiego
 *                           miasta podłączonego do odcinka;
 * @param length           - wskaźnik na zmienną z długością odcinka drogi;
 * @param year             - wskaźnik na zmienną z rokiem budowy/remontu drogi;
 */
void translateRoadDescription(CmdElement* city1, CmdElement** city2El,
                              unsigned* length, int* year);

/** @brief Konwertuje napis na zmienną typu unsigned int.
 * Konwertuje napis reprezentujący liczbę unsigned int na tę liczbę.
 * Funkcja powinna być wywoływana tylko, jeśli mamy pewność,
 * że reprezentowana liczba mieści się w zakresie unsigned.
 *
 * @param string               - napis reprezentujący liczbę;
 * @param length               - długość tego napisu;
 * @param isNegative           - informacja o tym, czy napis reprezentuje
 *                               liczbę ujemną;
 * @return Wartość liczby reprezentowanej w napisie.
 */
unsigned stringToUnsigned(const char* string, size_t length, bool isNegative);

/** @brief Konwertuje napis na liczbę całkowitą.
 * Konwertuje napis reprezentujący liczbę całkowitą na tę liczbę.
 * Funkcja powinna być wywoływana tylko, jeśli mamy pewność,
 * że reprezentowana liczba mieści się w zakresie int.
 *
 * @param string            - napis reprezentujący liczbę;
 * @param length            - długość tego napisu;
 * @return Wartość liczby reprezentowanej w napisie.
 */
int stringToInt(const char* string, size_t length);

/** @brief Przygotowuje strukturę z maksymalną wartością.
 * Przygotowuje strukturę z podaną maksymalną wartością.
 *
 * @param maxValue         - maksymalna wartość, którą chcemy
 *                           zapisać w strukturze;
 * @return Wskaźnik na strukturę z maksymalną wartością.
 */
MaxNumber* prepareMaxNumber(unsigned maxValue);

#endif //DROGI_TRANSLATORS_H
