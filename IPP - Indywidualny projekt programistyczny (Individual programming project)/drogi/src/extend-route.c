/** @file
 * Implementacje funkcji potrzebnych do funkcji extendRoute.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "extend-route.h"
#include "new-route.h"
#include "utils.h"

#include <stdlib.h>

/** @brief Sprawdza, czy miasto należy do drogi krajowej
 * Sprawdza, czy droga krajowa przechodzi przez podane miasto
 *
 * @param[in] cityStruc     - wskaźnik na sprawdzane miasto;
 * @param[in] route         - wskaźnik na sprawdzaną drogę;
 * @return Wartość @p jeśli miasto należy do drogi, @p false
 * jeśli nie należy
 */
static bool belongsToRoute(City* cityStruc, WholeRoute* route) {
    bool belongs;
    Route* segment;
    RoadSeg* current;

    belongs = false;
    segment = route->first;

    while (segment != NULL && !belongs) {
        current = segment->roadSeg;

        if (current->city1 == cityStruc || current->city2 == cityStruc)
            belongs = true;

        segment = segment->nextSegment;
    }

    return belongs;
}

bool prepareForExtension(Map* map, unsigned routeId, const char* city,
                         WholeRoute** route, City** cityStruc) {
    *route = findRoute(map, routeId);
    if (*route == NULL)
        return false;

    *cityStruc = findCity(map, city);
    if (*cityStruc == NULL)
        return false;

    if (belongsToRoute(*cityStruc, *route))
        return false;

    return true;
}

/** @brief Sprawdza, czy w candidates1 na pewno nie ma optymalnego wydłużenia
 * Sprawdza, czy w candidates1 na pewno nie ma optymalnego wydłużenia,
 * a w candidates2 być może jest
 *
 * @param[in] candidates1       - lista wydłużeń, w którym może nie być optymalnego;
 * @param[in] candidates2       - lista wydłużeń, w którym być może jest optymalne;
 * @return Wartość @p true, jeśli w candidates1 na pewno nie ma optymalnego wydłużenia,
 * a w candidates2 być może jest, @p false w przeciwnym przypadku
 */
static bool extensionCandidatesDiffer(RouteStack* candidates1, RouteStack* candidates2) {
    return candidates2 != NULL && (candidates1 == NULL ||
                                   candidates1->lengthSum >
                                   candidates2->lengthSum);
}

/** @brief Podaje ostatniego kandydata
 * Znajduje wskaźnik na ostatnie wydłużenie w liście kandydatów
 *
 * @param[in] candidates        - sprawdzana lista kandydatów;
 * @return Wskaźnik na ostatnie wydłużenie w liście kandydatów
 */
static RouteStack* scrollCandidates(RouteStack* candidates) {
    RouteStack *current, *next;

    current = candidates;
    next = current->nextCandidate;

    if (next != NULL) {
        while (next->first != NULL) {
            current = next;
            next = next->nextCandidate;
        }
    }

    return current;
}

/** @brief Porównuje wszystkich kandydatów z dwóch list kandydatów
 * Porównuje wszystkich kandydatów o jednakowej długości z dwóch list kandydatów
 *
 * @param[in] candidates1       - lista kandydatów 1;
 * @param[in] candidates2       - lista kandydató 2;
 * @return  Wskaźnik na "najmłodszego" kandydata, lub NULL jeśli nie da się takiego
 * wyznaczyć
 */
static WholeRoute* compareEqualCandidates(RouteStack* candidates1, RouteStack* candidates2) {
    RouteStack* connectionPoint;

    // Scalamy listy kandydatów
    connectionPoint = scrollCandidates(candidates1);
    free(connectionPoint->nextCandidate);
    connectionPoint->nextCandidate = candidates2;

    return bestCandidate(candidates1);
}

/** @brief Z wszystkich możliwości wybiera najlepsze wydłużenie
 * Z wszystkich możliwości wybiera najlepsze wydłużenie na podstawie długości
 * i wieku dróg
 *
 * @param[in] candidates1       - lista kandydatów z początku drogi
 * @param[in] candidates2       - lista kandydatów z końca drogi
 * @return Wskaźnik na najlepszegp kandydata, lub NULL jeśli nie da się takiego
 * wyznaczyć
 */
static WholeRoute* chooseExtenstion(RouteStack* candidates1, RouteStack* candidates2) {
    if (extensionCandidatesDiffer(candidates1,candidates2)) {
        clearCandidates(candidates1, NULL);
        return bestCandidate(candidates2);
    }
    else if (extensionCandidatesDiffer(candidates2, candidates1)) {
        clearCandidates(candidates2, NULL);
        return bestCandidate(candidates1);
    }
    else { // Kandydaci z obu końców mają równe długości
        return compareEqualCandidates(candidates1, candidates2);
    }
}

WholeRoute* findExtension(WholeRoute* route, City* city) {
    RouteStack *extCand1, *extCand2;
    WholeRoute* extension;

    // Potencjalne przedłużenie może zaczynać się albo na "końcu"
    // albo na "początku" drogi krajowej.
    extCand1 = findShortestCandidates(route->city1, city, NULL);
    extCand2 = findShortestCandidates(route->city2, city, NULL);
    // Jeśli nie istnieje przedłużenie ani z początku
    // ani z końca lub w środku funkcji nie udała się
    // alokacja pamięci.
    if (extCand1 == NULL && extCand2 == NULL)
        return NULL;

    extension = chooseExtenstion(extCand1, extCand2);

    return extension;
}

void appendExtension(WholeRoute* extension, WholeRoute* route, City* target) {
    Route* first;
    RoadSeg* firstSeg;
    City *start, *end;

    first = extension->first;
    firstSeg = first->roadSeg;
    start = route->city1;
    end = route->city2;

    if (firstSeg->city1 == end || firstSeg->city2 == end) {
        (route->last)->nextSegment = first;
        route->last = extension->last;
        route->city2 = target;
    }
    else if (firstSeg->city1 == start || firstSeg->city2 == start) {
        reverseRoute(extension);
        (extension->last)->nextSegment = route->first;
        route->first = extension->first;
        route->city1 = target;
    }
}