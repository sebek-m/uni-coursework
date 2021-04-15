/** @file
 * Funkcje potrzebne do implementacji funkcji removeRoad.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_REMOVE_ROAD_H
#define DROGI_REMOVE_ROAD_H

#include "structures.h"
#include "map.h"

/** @brief Usuwa podaną drogę
 * Usuwa podaną drogę i odłącza ją od miast, które łączy
 * oraz od listy wszystkich dróg.
 *
 * @param[in] map      – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] road     - usuwana droga;
 */
void onlyRemoveRoad(Map* map, RoadSeg* road);

/** @brief Uzupełnia drogi krajowe, które usunięcie odcinka przerwie
 * Jeśli to możliwe, uzupełnia drogi krajowe,
 * które zostaną przerwana przez usunięcie danego odcinka drogi.
 * Funkcja działa rekurencyjnie w celu zabezpieczenia się przed
 * uzupełnieniem jakiejś drogi zawierającej usuwany odcinek pomimo faktu,
 * że w innej drodze, która też zawiera ten odcinek, nie można
 * dokonać uzupełnienia;
 *
 * @param[in] targetRoad    - usuwany odcinek drogi;
 * @param[in] currentRoute  - aktualnie uzupełniana droga krajowa;
 * @return Wartość @p true jeśli udało się uzupełnić wszystkie dalsze drogi
 * krajowe zawierające dany odcinek, @p false w przeciwnym przypadku.
 */
bool fillContainingRoutes(RoadSeg* targetRoad, WholeRoute* currentRoute);

#endif //DROGI_REMOVE_ROAD_H
