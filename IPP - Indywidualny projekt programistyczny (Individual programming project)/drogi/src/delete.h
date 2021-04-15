/** @file
 * Funkcje służące do usuwania różnyc struktur.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_DELETE_H
#define DROGI_DELETE_H

#include "structures.h"
#include "map.h"

/** @brief Usuwa drogę krajową
 * Zwalnia pamięć pod wszystkich elementach listy drogi krajowej
 * i po samej strukturze drogi krajowej.
 *
 * @param[in] route     - usuwana droga krajowa
 */
void deleteWholeRoute(WholeRoute* route);

/** @brief Usuwa wszystkie odcinki dróg
 * Zwalnia pamięć po wszystkich odcinkach drogi w liście
 * wszystkich odcinków
 *
 * @param[in] map       - wskaźnik na strukturę mapy
 */
void deleteRoads(Map *map);

/** @brief Usuwa wszystkie miasta
 * Zwalnia pamięć po wszystkich miasta w liście
 * wszystkich miast
 *
 * @param[in] map       - wskaźnik na strukturę mapy
 */
void deleteCities(Map* map);

/** @brief Usuwa wszystkie drogi krajowe
 * Zwalnia pamięć po wszystkich drogach krajowych w liście
 * wszystkich dróg krajowych
 *
 * @param[in] map       - wskaźnik na strukturę mapy
 */
void deleteRoutes(Map* map);

#endif //DROGI_DELETE_H
