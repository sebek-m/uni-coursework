/** @file
 * Implementacje funkcji pomocniczych.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "utils.h"
#include "structures.h"

#include <stdlib.h>
#include <string.h>

City* findCity(Map* map, const char *city) {
    City *current;
    bool found;

    current = map->firstCity;
    found = false;

    while (current != NULL && !found) {
        if (strcmp(city, current->name) == 0)
            found = true;
        else
            current = current->nextCity;
    }

    return current;
}

bool validChar(char c) {
    return !((c >= 0 && c <= 31) || c == ';');
}

bool correctName(const char *cityName) {
    if (cityName == NULL || cityName[0] == '\0') {
        return false;
    }

    size_t length = strlen(cityName);
    bool correct = true;

    for (size_t i = 0; i < length && correct; i++) {
        if (!validChar(cityName[i]))
            correct = false;
    }

    if (correct && cityName[length] != '\0')
        correct = false;

    return correct;
}

City* otherCity(RoadSeg* road, City* otherCity) {
    if (road->city1 == otherCity)
        return road->city2;
    else
        return road->city1;
}

RoadSeg* findRoad(Map *map, const char *city1, const char *city2) {
    City* city1Struc;

    city1Struc = findCity(map, city1);
    if (city1Struc == NULL)
        return NULL;

    RoadSeg *targetRoad, **roads;
    const char* name;

    targetRoad = NULL;
    roads = city1Struc->roads;

    for (unsigned i = 0; i < city1Struc->roadsNum && targetRoad == NULL; i++) {
        RoadSeg* curr = roads[i];
        City* tmp = otherCity(curr, city1Struc);

        name = tmp->name;

        if (strcmp(city2, name) == 0)
            targetRoad = curr;
    }

    return targetRoad;
}

WholeRoute* findRoute(Map *map, unsigned routeId) {
    WholeRoute *current;

    current = map->firstRoute;

    while (current != NULL && current->routeId != routeId)
        current = current->nextWholeRoute;

    return current;
}

WholeRoute* scrollRoutes(Map* map, unsigned routeId) {
    WholeRoute *current, *next;

    current = map->firstRoute;
    if (current == NULL)
        return NULL;

    if (current->routeId == routeId)
        return current;

    next = current->nextWholeRoute;

    while (next != NULL && next->routeId != routeId) {
        current = next;
        next = next->nextWholeRoute;
    }

    if (next == NULL)
        return NULL;

    return current;
}

void markAllAsBelonging(WholeRoute* route) {
    Route* current;
    RoadSeg* currentSeg;

    current = route->first;

    while (current != NULL) {
        currentSeg = current->roadSeg;

        if (!(currentSeg->belongsToRoute))
            currentSeg->belongsToRoute = true;

        current = current->nextSegment;
    }
}

void markAllAsVisited(WholeRoute* route) {
    Route* current;
    City *city1, *city2;

    current = route->first;

    while (current != NULL) {
        city1 = (current->roadSeg)->city1;
        city2 = (current->roadSeg)->city2;

        if (!(city1->visited))
            city1->visited = true;
        if (!(city2->visited))
            city2->visited = true;

        current = current->nextSegment;
    }
}

void markAllAsUnvisited(WholeRoute* route) {
    Route* current;
    City *city1, *city2;

    current = route->first;

    while (current != NULL) {
        city1 = (current->roadSeg)->city1;
        city2 = (current->roadSeg)->city2;

        if (city1->visited)
            city1->visited = false;
        if (city2->visited)
            city2->visited = false;

        current = current->nextSegment;
    }
}

void reverseRoute(WholeRoute* route) {
    Route *current, *next, *previous;

    previous = NULL;
    current = route->first;

    while (current != NULL) {
        next = current->nextSegment;

        current->nextSegment = previous;

        previous = current;
        current = next;
    }

    route->last = route->first;
    route->first = previous;
}

/** @brief Sprawdza, czy odcinek drogi należy do jakiejkolwiek drogi krajowej.
 * Sprawdza, czy odcinek drogi należy do jakiejkolwiek drogi krajowej.
 *
 * @param road               - sprawdzany odcinek drogi;
 * @param map                - wskaźnik na strukturę mapy;
 * @return Wartość @p true, jeśli należy do jakiejś drogi, @p false w przeciwnym przypadku.
 */
static bool belongsToAnyRoute(RoadSeg* road, Map* map) {
    WholeRoute* current;
    Route* currentSeg;
    bool belongs;

    current = map->firstRoute;
    belongs = false;

    while (current != NULL) {
        currentSeg = current->first;

        while (currentSeg != NULL && currentSeg->roadSeg != road)
                currentSeg = currentSeg->nextSegment;

        if (currentSeg != NULL)
                belongs = true;

        current = current->nextWholeRoute;
    }

    return belongs;
}

void tidySharedSegments(WholeRoute* route, Map* map) {
    Route* current;
    RoadSeg* currentSeg;

    current = route->first;

    while (current != NULL) {
        currentSeg = current->roadSeg;

        if (!belongsToAnyRoute(currentSeg, map))
            currentSeg->belongsToRoute = false;

        current = current->nextSegment;
    }
}