/** @file
 * Implementacje funkcji potrzebnych do implementacji funkcji newRoute.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "new-route.h"
#include "utils.h"

#include <stdlib.h>

void checkAllNext(RouteStack* stack, RouteStack** candidates,
                         City* origin, RoadSeg* current, bool* memoryLeft) {
    RoadSeg **roads, *next;

    roads = origin->roads;
    // Żeby uniknąć samoprzecięć
    origin->visited = true;

    for (unsigned i = 0; i < origin->roadsNum && *memoryLeft; i++) {
        next = roads[i];

        if (next != current) {
            *memoryLeft = push(stack, next);
            // Tak naprawdę rekurencyjne wywołanie
            // checkCurrent z dodanym odcinkiem
            if (*memoryLeft)
                checkCurrent(stack, candidates, origin, memoryLeft);

            // "Wycofujemy" się z aktualnie sprawdzanej drogi w tył
            pop(stack);
        }
    }

    origin->visited = false;
}

void evaluateIfCandidate(RouteStack* stack, RouteStack** candidates,
                                bool* memoryLeft) {
    if (empty(*candidates) ||
        stack->lengthSum == (*candidates)->lengthSum) {
        *memoryLeft = addCandidate(stack, candidates);
    }
    else if (stack->lengthSum < (*candidates)->lengthSum) {
        removePreviousCandidates(candidates);
        *memoryLeft = addCandidate(stack, candidates);
    }
}

void checkCurrent(RouteStack* stack, RouteStack** candidates,
                         City* origin, bool* memoryLeft) {
    City* nextCity;
    RoadSeg* current;

    current = ((stack->first)->roadSeg);
    nextCity = otherCity(current, origin);

    // Drugi warunek to zabezpieczenie przed sprawdzeniem
    // usuwanej drogi w przypadku użycia funkcji
    // w removeRoad
    if (!(nextCity->visited) &&
        (stack->last)->roadSeg != stack->targetRoad) {
        if (nextCity == stack->target) {
            evaluateIfCandidate(stack, candidates, memoryLeft);
        }
        else if (empty(*candidates) ||
                 stack->lengthSum < (*candidates)->lengthSum) {
            checkAllNext(stack, candidates, nextCity, current, memoryLeft);
        }
    }
}

RouteStack* findShortestCandidates(City* city1, City* city2, RoadSeg* illegalRoad) {
    RouteStack *active, *candidates;
    bool memoryLeft;

    active = createStack(city2, illegalRoad);
    if (active == NULL)
        return NULL;
    candidates = createStack(city2, illegalRoad);
    if (candidates == NULL) {
        free(active);
        return NULL;
    }

    memoryLeft = true;

    checkAllNext(active, &candidates, city1, NULL, &memoryLeft);

    free(active);

    // Nie istnieje droga, z której można utworzyć drogę krajową
    // lub nie udało się zaalokować pamięci w trakcie szukania.
    if (empty(candidates) || !memoryLeft) {
        clearCandidates(candidates, NULL);
        return NULL;
    }

    return candidates;
}

WholeRoute* bestCandidate(RouteStack* candidates) {
    RouteStack* winner;
    WholeRoute* ret;

    winner = chooseWinner(candidates);
    // Jeśli nie udało się jednoznacznie wyznaczyć
    // dobrej drogi krajowej z najkrótszych dróg,
    // ponieważ we wszystkich najstarszy odcinek
    // był z tego samego roku
    if (winner == NULL) {
        clearCandidates(candidates, NULL);
        return NULL;
    }

    ret = transform(winner);
    if (!ret)
        clearCandidates(candidates, NULL);
    else
        clearCandidates(candidates, winner);

    return ret;
}

WholeRoute* selectShortestRoute(City* city1, City* city2) {
    RouteStack* candidates;
    WholeRoute* new;

    candidates = findShortestCandidates(city1, city2, NULL);
    if (candidates == NULL) {
        clearCandidates(candidates, NULL);
        return NULL;
    }

    new = bestCandidate(candidates);

    return new;
}

void finalizeRoute(Map* map, WholeRoute* newRoute, unsigned routeId,
                   City* city1, City* city2) {
    newRoute->routeId = routeId;
    newRoute->city1 = city1;
    newRoute->city2 = city2;
    markAllAsBelonging(newRoute);
    newRoute->nextWholeRoute = map->firstRoute;
    map->firstRoute = newRoute;
}