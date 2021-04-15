/** @file
 * Funkcje potrzebne do implementacji funkcji addRoad.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_ADD_ROAD_H
#define DROGI_ADD_ROAD_H

#include "map.h"

#include <stdlib.h>

/** @brief Główna część funkcji addRoad
 * Funkcja wywoływana w funkcji addRoad, gdy w podanych parametrach nie ma błędów.
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param length         - długość, jaką ma mieć nowa droga;
 * @param builtYear      - rok budowy nowej drogi;
 * @return Wartość @p true, jeśli dodawanie nowej drogi zakończyło się pomyślnie,
 * @p false, jeśli wystąpił jeden z błędów opisanych przy addRoad;
 */
bool initializeRoad(Map *map, const char *city1, const char *city2,
                    unsigned length, int builtYear);

/** @brief Dodaje drogę lub naprawia ją jeśli istnieje
 * Tworzy zadaną drogę, jeśli jest to możliwe, lub naprawia
 * ją. Jeśli miasta nie istnieją, to je też tworzy.
 *
 * @param map                      - wskaźnik na mapę;
 * @param city1                    - wskaźnik na napis będący nazwą miasta 1;
 * @param city2                    - wskaźnik na napis będący nazwą miasta 2;
 * @param length                   - oczekiwana długość drogi;
 * @param year                     - oczekiwany rok budowy;
 * @param citiesAdded              - zmienna zliczająca miasta dodane w przebiegu
 *                                   funkcji newRouteFromDescription
 * @param roadsAdded               - zmienna zliczająca miasta dodane w przebiegu
 *                                   funkcji newRouteFromDescription
 * @return Wskaźnik na stworzoną lub naprawioną drogę. NULL w przypadku błędu.
 */
RoadSeg* addOrRepairRoad(Map *map, const char *city1, const char *city2,
                         unsigned length, int year, size_t* citiesAdded,
                         size_t* roadsAdded);

#endif //DROGI_ADD_ROAD_H
