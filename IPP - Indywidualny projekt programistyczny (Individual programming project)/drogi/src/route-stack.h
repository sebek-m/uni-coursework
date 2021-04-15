/** @file
 * Pomocnicza struktura specjalnego stosu fragmentów
 * drogi, potrzebna do obsługi map, oraz funkcje
 * potrzebne do obsługi tej struktury.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_ROUTE_STACK_H
#define DROGI_ROUTE_STACK_H

#include <stdbool.h>
#include "structures.h"

/**
 * Stos, reprezentujący drogę, który służy
 * do szukania optymalnej drogi i wygodnego
 * tworzenia drogi z istniejących odcinków.
 */
typedef struct RouteStack RouteStack;

/**
 * Stos, reprezentujący drogę, który służy
 * do szukania optymalnej drogi i wygodnego
 * tworzenia drogi z istniejących odcinków.
 */
struct RouteStack {
    Route *first; /**< wskaźnik na pierwszy element listy odcinków drogi */
    Route *last; /**< wskaźnik na ostatni element listy odcinków drogi */
    unsigned long lengthSum; /**< suma długości odcinków na stosie */
    RouteStack* nextCandidate; /**< następny stos w liście kandydatów na optymalną drogę */
    City* target; /**< miasto, do którego będzie prowadzić droga */
    RoadSeg* targetRoad; /**< odcinek drogi, który w przypadku removeRoad chcemy usunąć */
};

/** @brief Tworzy stos odcinków.
 * Tworzy pusty stos odcinków reprezentujący drogę.
 *
 * @param[in] target     - miasto, do którego będzie prowadzić droga;
 * @param[in] targetRoad - odcinek drogi, którą chcemy usunąć;
 * @return Wskaźnik na stos. Null, jeśli alokacja pamięci sie nie powiodła.
 */
RouteStack* createStack(City* target, RoadSeg* targetRoad);

/** @brief Wstawia odcinek drogi na stos.
 * Wstawia odcinek drogi na stos i dodaje jego długość do sumy.
 *
 * @param[in] stack     - stos;
 * @param[in] road      - droga;
 * @return Wartość @p true, jeśłi się udało, @p false, jeśli alokacja
 * pamięci sie nie powiodła
 */
bool push(RouteStack* stack, RoadSeg* road);

/** @brief Zdejmuje pierwszy odcinek ze stosu.
 * Zdejmuje ostatnio dodany odcinek ze stosu i zwraca na niego wskaźnik.
 *
 * @param[in] stack     - stos;
 * @return Wskaźnik na zdejmowany odcinek
 */
RoadSeg* pop(RouteStack* stack);

/** @brief Sprawdza, czy stos jest pusty.
 * Sprawdza, czy stos jest pusty.
 *
 * @param[in] stack     - stos;
 * @return Wartość @p true jeśłi pusty, @p false jeśli niepusty;
 */
bool empty(RouteStack* stack);

/** @brief Usuwa stos.
 * Usuwa stos.
 * @param[in] stack     - stos;
 */
void clear(RouteStack* stack);

/** @brief Usuwa wszystkich kandydatów z listy.
 * Usuwa wszystkie elementy listy kandydatów poza strażnikiem.
 *
 * @param[in,out] candidates    - lista kandydatów;
 */
void removePreviousCandidates(RouteStack** candidates);

/** @brief Kopiuje stos.
 * Alokuje pamięć na kopię stosu i ją tworzy.
 *
 * @param[in] stack     - kopiowany stos;
 * @return Wskaźnik na kopię lub NULL, gdy alokacja pamięci się nie powiodła.
 */
RouteStack* copyRouteStack(RouteStack* stack);

/** @brief Dodaje nowego kandydata do listy.
 * Dodaje podaną drogę (stos) do listy kandydatów.
 *
 * @param[in] stack             - nowy kandydat;
 * @param[in,out] candidates    - lista kandydatów;
 * @return Wartość @p true, jeśli się udało, @p false
 * jeśli błąd alokacji pamięci.
 */
bool addCandidate(RouteStack* stack, RouteStack** candidates);

/** @brief Przekształca stos w prawdziwą drogę.
 * Alokuje pamięć na nową drogę i przekształca w nią stos.
 *
 * @param[in] stack     - stos;
 * @return Wskaźnik na nową drogę, lub NULL jeśli alokacja sie nie powiodła.
 */
WholeRoute* transform(RouteStack* stack);

/** @brief Czyści listę kandydatów.
 * Usuwa wszystkie drogi i strażnika z listy kandydatów, poza podaną drogą.
 *
 * @param[in] candidates        - lista kandydatów;
 * @param[in] winner            - droga, która ma nie być usutnięta;
 */
void clearCandidates(RouteStack* candidates, RouteStack* winner);

/** @brief Wyznacza z kandydatów optymalną drogę.
 * Wyznacza wśród kandydatów o jednakowej długości drogę o najmłodszym
 * z najstarszych odcinków.
 *
 * @param[in] candidates    - lista kandydatów;
 * @return Wskaźnik na optymalną drogę lub NULL jeśli nie da sie jej wyznaczyć.
 */
RouteStack* chooseWinner(RouteStack* candidates);

/** @brief Wybiera poprawne pierwsze i ostatnie miasto ze stosu.
 * Przypisuje odpowiednie wartości zmiennym city1 i city2,
 * żeby było wiadomo, jak je podłączyć do struktury WholeRoute.
 *
 * @param stack             - stos z odcinkami dróg;
 * @param city1             - wskaźnik na wskaźnik do struktury miasta 1;
 * @param city2             - wskaźnik na wskaźnik do struktury miasta 2;
 * @param city1Name         - nazwa miasta 1;
 * @param city2Name         - nazwa miasta 2;
 */
void recognizeCities(RouteStack* stack, City** city1,
                     City** city2, const char* city1Name,
                     const char* city2Name);

/** @brief Odwraca stos.
 * Odwraca listę reprezentującą drogę w formie stosu.
 *
 * @param[in] stack     - wskaźnik na odwracany stos;
 */
void reverseStack(RouteStack* stack);

#endif //DROGI_ROUTE_STACK_H
