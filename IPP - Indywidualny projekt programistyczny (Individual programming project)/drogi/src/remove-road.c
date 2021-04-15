/** @file
 * Implementacje funkcji do implementacji funkcji removeRoad.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "remove-road.h"
#include "utils.h"
#include "route-stack.h"
#include "delete.h"
#include "new-route.h"

#include <stdlib.h>
#include <stdio.h>

/** @brief Znajduje w liście odcinków szukany odcinek.
 * Znajduje w liście odcinków szukany odcinek.
 *
 * @param[in] first     - wskaźnik na pierwszy odcinek listy;
 * @param[in] target    - wskaźnik na szukany odcinek;
 * @return Wskaźnik na odcinek poprzedzający ten szukany.
 */
static RoadSeg* scrollRoadSeg(RoadSeg* first, RoadSeg* target) {
    RoadSeg *current, *next;

    current = first;
    next = current->nextRoadSeg;

    if (current != target) {
        while (next != NULL && next != target) {
            current = next;
            next = next->nextRoadSeg;
        }
    }

    return current;
}

/** @brief Odłącza drogę od miasta.
 * Odłącza podany odcinek drogi od miasta.
 *
 * @param[in] city      - obsługiwane miasto;
 * @param[in] road      - odłączana droga;
 */
static void disconnectRoad(City* city, RoadSeg* road) {
    unsigned roadNum, pos;
    RoadSeg** roads;

    roadNum = city->roadsNum;
    roads = city->roads;

    for (pos = 0; pos < roadNum && roads[pos] != road; pos++);

    roads[pos] = NULL;

    for (unsigned i = pos; i < roadNum - 1; i++)
        roads[i] = roads[i + 1];

    (city->roadsNum)--;
}

void onlyRemoveRoad(Map* map, RoadSeg* road) {
    RoadSeg* previous;
    City *city1, *city2;

    // Zdejmujemy usuwany odcinek z listy wszystkich odcinków
    previous = scrollRoadSeg(map->firstRoad, road);
    previous->nextRoadSeg = road->nextRoadSeg;

    city1 = road->city1;
    city2 = road->city2;

    disconnectRoad(city1, road);
    disconnectRoad(city2, road);

    if (map->firstRoad == road)
        map->firstRoad = road->nextRoadSeg;

    free(road);
}

/** @brief Ustawia początek i koniec przerwy.
 * Ustawia zmienne origin i end na adresy odpowiednich miast,
 * zależnie od tego skąd usuwany jest odcinek.
 *
 * @param[in] route       - sprawdzana droga;
 * @param[in] previous    - wskaźnik na odcinek drogi poprzedzający ten usuwany;
 * @param[in,out] origin  - wskaźnik na wskaźnik na miasto (początek przerwy);
 * @param[in,out] end     - wskaźnik na wskaźnik na miasto (koniec przerwy);
 * @param[in] road        - usuwany odcinek;
 */
static void setOriginAndEnd(WholeRoute* route, Route* previous,
                            City** origin, City** end, RoadSeg* road) {
    // Jeśli pierwszy odcinek drogi ma być usunięty.
    if (!previous) {
        // Jeśli droga ma tylko jeden odicnek
        if (route->first == route->last) {
            *origin = route->city1;
            *end = route->city2;
        }
        else if (road->city1 == route->city1){
            *origin = road->city1;
            *end = road->city2;
        }
        else {
            *origin = road->city2;
            *end = road->city1;
        }
    }
    else if ((previous->roadSeg)->city2 == road->city1 ||
             (previous->roadSeg)->city1 == road->city1) {
        *origin = road->city1;
        *end = road->city2;
    }
    else {
        *origin = road->city2;
        *end = road->city1;
    }
}

/** @brief Sprawdza, czy podany odcinek jest w podanej drodze.
 * Sprawdza, czy podany odcinek jest w podanej drodze i, jeśli tak,
 * ustawia wartości zmiennych city1 i city2 na adresy początku i końca
 * ewentualnej przerwy, która powstanie w drodze po usunięciu odcinka.
 *
 * @param[in] route         - sprawdzana droga;
 * @param[in] road          - usuwany odcinek drogi;
 * @param[in, out] city1    - wskaźnik na wskaźnik na miasto (początek przerwy);
 * @param[in, out] city2    - wskaźnik na wskaźnik na miasto (koniec przerwy);
 * @return Wartość @p true, jeśli jeśli odcinek należy do drogi, @p false
 * jeśli nie należy.
 */
static bool roadInRoute(WholeRoute* route, RoadSeg* road,
                        City** city1, City** city2) {
    Route *currentSeg, *previous;

    currentSeg = route->first;
    previous = NULL;

    while (currentSeg != NULL && (currentSeg->roadSeg) != road) {
        previous = currentSeg;
        currentSeg = currentSeg->nextSegment;
    }

    if (currentSeg == NULL)
        return false;

    setOriginAndEnd(route, previous, city1, city2, road);

    return true;
}

/** @brief Wstawia uzupełnienie do drogi.
 * Wstawia uzupełnienie w odpowiednie miejsce w drodze,
 * w różny sposób zależnie od miejsca, z którego był usuwany odcinek.
 *
 * @param[in] route     - uzupełniana droga;
 * @param[in] filling   - uzupełnienie;
 * @param[in] road      - usuwana droga;
 */
static void insertFilling(WholeRoute* route, WholeRoute* filling, RoadSeg* road) {
    Route *current, *previous;

    current = route->first;
    previous = NULL;

    // Mamy pewność, że w obsługiwanej drodze krajowej znajduje
    // się docelowy odcinek, więc nie przejmujemy się napotkaniem
    // NULLa.
    while ((current->roadSeg) != road) {
        previous = current;
        current = current->nextSegment;
    }

    // Jeśli usuwany był pierwszy odcinek
    if (!previous) {
        if (route->first == route->last) {
            route->first = filling->first;
            route->last = filling->last;
        }
        else {
            route->first = filling->first;
            (filling->last)->nextSegment = current->nextSegment;
        }
    } // Jeśli usuwany był ostatni odcinek
    else if (current->nextSegment == NULL) {
        previous->nextSegment = filling->first;
        route->last = filling->last;
    }
    else {
        previous->nextSegment = filling->first;
        (filling->last)->nextSegment = current->nextSegment;
    }

    free(current);
}

/** @brief Znajduje optymalne wypełnienie drogi.
 * Jeśli to możliwe, znajduje optymalne uzupełnienie drogi
 * po usuwanym odcinku drogi.
 *
 * @param[in] route       - uzupełniana droga;
 * @param[in] city1       - początek przerwy w drodze;
 * @param[in] city2       - koniec przerwy w drodze;
 * @param[in] illegalRoad - odcinek do usunięcia, czyli ten,
 *                          którego nie chcemy uwzględniać
 *                          w rozważanym uzupelnieniu;
 * @return Wskaźnik na optymalne uzupełnienie lub NULL jeśli
 * nie da się takiego wyznaczyć.
 */
static WholeRoute* getFilling(WholeRoute* route, City* city1,
                              City* city2, RoadSeg* illegalRoad) {
    WholeRoute* filling;
    RouteStack* candidates;

    markAllAsVisited(route);
    city2->visited = false;

    candidates = findShortestCandidates(city1, city2, illegalRoad);
    if (candidates == NULL) {
        clearCandidates(candidates, NULL);
        filling = NULL;
    }
    else {
        filling = bestCandidate(candidates);
    }

    markAllAsUnvisited(route);

    return filling;
}

bool fillContainingRoutes(RoadSeg* targetRoad, WholeRoute* currentRoute) {
    if (currentRoute == NULL) {
        return true;
    }
    else {
        City *city1, *city2;
        WholeRoute *filling;
        bool noError;

        if (roadInRoute(currentRoute, targetRoad, &city1, &city2)) {
            filling = getFilling(currentRoute, city1, city2, targetRoad);
            if (!filling) {
                noError = false;
            }
            else {
                noError = fillContainingRoutes(targetRoad, currentRoute->nextWholeRoute);
                if (noError) {
                    insertFilling(currentRoute, filling, targetRoad);
                    markAllAsBelonging(filling);
                    free(filling);
                }
                else {
                    deleteWholeRoute(filling);
                }
            }
        }
        else {
            noError = fillContainingRoutes(targetRoad, currentRoute->nextWholeRoute);
        }

        return noError;
    }
}
