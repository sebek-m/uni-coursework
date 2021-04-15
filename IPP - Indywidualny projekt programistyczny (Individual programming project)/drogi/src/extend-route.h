/** @file
 * Funkcje potrzebne do implementacji funkcji extendRoute.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_EXTEND_ROUTE_H
#define DROGI_EXTEND_ROUTE_H

#include "structures.h"
#include "map.h"

/** @brief Przygotowuje funkcję extendRoute do działania
 * Sprawdza, czy istnieje podana droga i miasto oraz
 * przypisuje odpowiednim zmiennym wartości.
 *
 * @param[in] map               – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId           - numer wydłużanej drogi krajwoej
 * @param[in] city              - wskaźnik na napis, będący nazwą miasta
 * @param[in, out] route        - wskaźnik na wskaźnik na wydłużaną drogę krajową
 * @param[in, out] cityStruc    - wskaźnik na wskaźnik na miasto, do którego
 *                                jest wydłużana droga
 * @return Wartość @p true, jeśli istnieje podana droga i miasto, oraz miasto nie należy
 * do drogi, @p false w przeciwnym przypadku
 */
bool prepareForExtension(Map* map, unsigned routeId, const char* city,
                         WholeRoute** route, City** cityStruc);

/** @brief Znajduje odpowiednie wydłużenie drogi
 * Znajduje najkrótsze wydłużenie drogi do podanego miasta
 *
 * @param[in] route     - wydłużana droga krajowa;
 * @param[in] city      - miasto, będące celem wydłużenia;
 * @return Wskaźnik na drogę, która jest wydłużeniem/NULL jeśli
 * nie udało się znaleźć wydłużenia, lub nie udała się jakaś alokacja pamięci
 */
WholeRoute* findExtension(WholeRoute* route, City* city);

/** @brief Podłącza wydłużenie do drogi krajowej
 * Scala drogę krajową z wydłużeniem.
 *
 * @param[in] extension     - wskaźnik na wydłużenie;
 * @param[in] route         - wskaźnik na wydłużaną drogę;
 * @param[in] target        - wskaźnik na miasto - cel wydłużenia
 */
void appendExtension(WholeRoute* extension, WholeRoute* route, City* target);

#endif //DROGI_EXTEND_ROUTE_H
