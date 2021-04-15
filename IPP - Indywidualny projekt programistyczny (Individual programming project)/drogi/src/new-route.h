/** @file
 * Funkcje potrzebne do implementacji funkcji newRoute.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_NEW_ROUTE_H
#define DROGI_NEW_ROUTE_H

#include "structures.h"
#include "route-stack.h"
#include "map.h"

#include <stdbool.h>

/** @brief Sprawdza drogę z dodanymi kolejnymi odcinkami
 * Po kolei dołącza po jednym odcinku z podłączonych do miasta
 * będącego na końcu aktualnego stanu drogi i wywołuje checkCurrent,
 * żeby dalej rekurencyjnie szukać najkrótszej drogi.
 *
 * @param[in] stack             - sprawdzana droga (stos odcinków);
 * @param[in, out] candidates   - wskaźnik na listę
 *                                aktualnie najkrótszych potencjalnych dróg;
 * @param[in] origin            - miasto, z którego wychodzą sprawdzane odcinki;
 * @param[in] current           - wskaźnik na ostatnio dodany odcinek drogi;
 * @param[in, out] memoryLeft   - informacja, czy wszystkie alokacje się powodząl;
 */
void checkAllNext(RouteStack* stack, RouteStack** candidates,
                  City* origin, RoadSeg* current, bool* memoryLeft);

/** @brief Decyduje, czy sprawdzana droga jest nową najkrótszą
 * Sprawdza, czy sprawdzana droga jest nową najkrótszą, która
 * prowadzi do celu. Jeśli tak, to usuwa poprzednich kandydatów
 * i dodaje ją jako nowego.
 *
 * @param[in] stack             - sprawdzana droga (stos odcinków);
 * @param[in, out] candidates   - wskaźnik na listę
 *                                aktualnie najkrótszych potencjalnych dróg;
 * @param[in, out] memoryLeft   - informacja, czy wszystkie alokacje się powodząl;
 */
void evaluateIfCandidate(RouteStack* stack, RouteStack** candidates,
                         bool* memoryLeft);

/** @brief Decyduje, czy aktualnie sprawdzana droga ma szanszę na bycie najkrótszą.
 * W połączeniu z checkAllNext, rekurencyjnie sprawdza wszystkie potencjalne drogi
 * w grafie wychodzące z miasta origin i decyduje, czy po ewentualnym dodaniu kolejnych
 * istniejących odcinków do sprawdzanej drogi, będzie ona miała szansę na bycie najkrótszą,
 * która dochodzi do docelowego miasta oraz czy sprawdzana droga już ma szansę na bycie najkrótszą
 * prawidłową.
 *
 * @param[in] stack              - aktualnie sprawdzana droga (stos odcinków);
 * @param[in, out] candidates    - wskaźnik na listę
 *                                 aktualnie najkrótszych potencjalnych dróg;
 * @param[in] origin             - miasto, z którego ma wychodzić docelowa droga;
 * @param[in, out] memoryLeft    - informacja, czy wszystkie alokacje się powodzą;
 */
void checkCurrent(RouteStack* stack, RouteStack** candidates,
                  City* origin, bool* memoryLeft);

/** @brief Znajduje najkrótsze możliwe drogi między dwoma miastami
 * Znajduje zbiór dróg o jednakowej, najkrótszej możliwej długości,
 * które łączą podane miasta
 *
 * @param[in] city1         - wskaźnik na miasto 1;
 * @param[in] city2         - wskaźnik na miasto 2;
 * @param[in] illegalRoad   - odcinek do usunięcia w przypadku użycia przy removeRoad;
 * @return Wskaźnik na listę kandydatów lub NULL w przypadku niepowodzenia
 */
RouteStack* findShortestCandidates(City* city1, City* city2, RoadSeg* illegalRoad);

/** @brief Wybiera "najmłodszą" drogę
 * Z dróg o jednakowej długości wybiera tę o najmłodszym z najstarszych
 * odcinków drogi
 *
 * @param[in] candidates    - lista potencjalnie najlepszych dróg
 * @return Wskaźnik na najlepszą drogę, lub NULL w przypadku niepowodzenia
 * szukania
 */
WholeRoute* bestCandidate(RouteStack* candidates);

/** @brief Znajduje najkrótszą drogę pomiędzy miastami
 * Wywołuje funkcje wymagane do znalezienia najkrótszej drogi pomiędzy
 * podanymi miastami
 *
 * @param[in] city1     - wskaźnik na pierwsze miasto;
 * @param[in] city2     - wskaźnik na drugie miasto;
 * @return Wskaźnik na najkrótszą drogę lub NULL w przypadku niepowodzenia
 */
WholeRoute* selectShortestRoute(City* city1, City* city2);

/** @brief Dokańcza proces tworzenia nowej drogi krajowej
 * Uzupełnia brakujące pola i wstawia drogę krajową do mapy
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] newRoute   - wskaźnik na utworzoną drogę;
 * @param[in] routeId    - numer drogi;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 */
void finalizeRoute(Map* map, WholeRoute* newRoute, unsigned routeId,
                   City* city1, City* city2);

#endif //DROGI_NEW_ROUTE_H
