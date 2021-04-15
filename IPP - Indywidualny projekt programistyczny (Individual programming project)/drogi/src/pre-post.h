/** @file
 * Funkcje potrzebne do przygotowania programu do działania
 * oraz do przygotowania go do zakończenia.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_PRE_POST_H
#define DROGI_PRE_POST_H

#include "map.h"
#include "translators.h"

#include <stdbool.h>

/** @brief Operacje przygotowujące
 * Tworzy mapę oraz struktury z maksymalnymi wartościami
 * typów liczbowych.
 *
 * @param map           - wskaźnik na wskaźnik na mapę;
 * @param maxU          - wskaźnik na wskaźnik na reprezentację
 *                        maksymalnego unsigned int;
 * @param maxInt        - wskaźnik na wskaźnik na reprezentację
 *                        maksymalnego int;
 * @return Wartość @p true jeśli wszystkie alokacje się powiodły,
 * @p false, w przeciwnym przypadku.
 */
bool pre(Map** map, MaxNumber** maxU, MaxNumber** maxInt);

/** @brief Operacje przygotowujące do zakonczenia programu
 * Usuwa mapę i struktury z maksymalnymi wartościami
 *
 * @param map           - wskaźnik na mapę;
 * @param maxU          - wskaźnik nareprezentację
 *                        maksymalnego unsigned int;
 * @param maxInt        - wskaźnik na reprezentację
 *                        maksymalnego int;
 */
void post(Map* map, MaxNumber* maxU, MaxNumber* maxInt);

#endif //DROGI_PRE_POST_H
