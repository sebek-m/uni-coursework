/** @file
 * Funkcje potrzebne do czytania komend ze standardowego wejścia
 * oraz do ich interpretacji.
 *
 * @author Sebastian Miller
 * @date 16.05.2019
 */
#ifndef DROGI_CMD_READER_H
#define DROGI_CMD_READER_H

#include "map.h"
#include "helper-strucs.h"

#include <stdlib.h>
#include <stdbool.h>

/** @brief Czyta i interpretuje polecenie z wejścia
 * Czyta, sprawdza czy jest poprawna oraz interpretuje i wykonuje jedną linię
 * polecenia z wejścia.
 *
 * @param map                - wskaźnik na mapę;
 * @param maxU               - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt             - wskaźnik na strukturę z maksymalnym intem;
 * @param eofGuard           - wskaźnik na zmienną informującą, czy napotkano EOF;
 * @return Wartość @p true w przypadku braku błędu, @p false w przeciwnym przypadku.
 */
bool readAndInterpret(Map* map, MaxNumber* maxU, MaxNumber* maxInt, bool* eofGuard);

/** @brief Wypisuje informację o błędzie, jeśli wystąpił.
 * Wypisuje informację o błędzie, jeśli wystąpił.
 *
 * @param noError       - informacja o błędzie;
 * @param lineNum       - numer przeczytanej linii;
 */
void checkError(bool noError, size_t lineNum);

#endif //DROGI_CMD_READER_H
