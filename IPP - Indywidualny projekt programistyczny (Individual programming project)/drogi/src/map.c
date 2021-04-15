/** @file
 * Implementacje funkcji do obsługi map krajowych.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "map.h"
#include "structures.h"
#include "route-stack.h"
#include "utils.h"
#include "delete.h"
#include "add-road.h"
#include "new-route.h"
#include "extend-route.h"
#include "remove-road.h"
#include "route-description.h"
#include "helper-strucs.h"
#include "cmd-reader.h"
#include "translators.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Minimalny numer drogi
 */
#define MIN_ROUTE_ID 1
/**
 * Maksymalny numer drogi
 */
#define MAX_ROUTE_ID 999

Map* newMap(void) {
    Map* newMap;

    newMap = (Map*)malloc(sizeof(Map));

    if (newMap) {
        newMap->firstCity = NULL;
        newMap->firstRoute = NULL;
        newMap->firstRoad = NULL;
    }

    // Korzystamy z faktu, że nieudany malloc zwraca NULL.
    return newMap;
}

void deleteMap(Map *map) {
    if (map) {
        deleteRoads(map);
        deleteCities(map);
        deleteRoutes(map);
        free(map);
    }
}

bool addRoad(Map *map, const char *city1, const char *city2,
             unsigned length, int builtYear) {
    if (map == NULL || length < 1 || builtYear == 0) {
        return false;
    }
    else if (!correctName(city1) || !correctName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    else {
        return initializeRoad(map, city1, city2, length, builtYear);
    }
}

bool repairRoad(Map *map, const char *city1,
                const char *city2, int repairYear) {
    if (map == NULL || repairYear == 0) {
        return false;
    }
    else if (!correctName(city1) || !correctName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    else {
        RoadSeg* targetRoad = findRoad(map, city1, city2);
        if (targetRoad == NULL)
            return false;

        if (repairYear < targetRoad->lastRepairYear)
            return false;

        targetRoad->previousRepairYear = targetRoad->lastRepairYear;
        targetRoad->lastRepairYear = repairYear;

        return true;
    }
}

bool newRoute(Map *map, unsigned routeId,
              const char *city1, const char *city2) {
    if (map == NULL || routeId < MIN_ROUTE_ID || routeId > MAX_ROUTE_ID) {
        return false;
    }
    else if (findRoute(map, routeId) != NULL) {
        return false;
    }
    else if (!correctName(city1) || !correctName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    else {
        City *city1Struc, *city2Struc;
        WholeRoute* new;

        city1Struc = findCity(map, city1);
        if (city1Struc == NULL)
            return false;

        city2Struc = findCity(map, city2);
        if (city2Struc == NULL)
            return false;

        new = selectShortestRoute(city1Struc, city2Struc);
        if (new == NULL)
            return false;

        finalizeRoute(map, new, routeId, city1Struc, city2Struc);

        return true;
    }
}

bool extendRoute(Map *map, unsigned routeId, const char *city) {
    if (map == NULL || routeId < MIN_ROUTE_ID || routeId > MAX_ROUTE_ID) {
        return false;
    }
    else if (!correctName(city)) {
        return false;
    }
    else {
        City *cityStruc;
        WholeRoute *extension, *route;

        if (!prepareForExtension(map, routeId, city, &route, &cityStruc))
            return false;

        markAllAsVisited(route);

        extension = findExtension(route, cityStruc);
        // Nie udało się jednoznacznie wyznaczyć przedłużenia
        // lub nie powiodła się alokacja pamięci.
        if (extension == NULL)
            return false;

        markAllAsBelonging(extension);
        appendExtension(extension, route, cityStruc);
        markAllAsUnvisited(route);

        // Lista odcinków podpięta pod drogę zostaje,
        // ale musimy usunąć strukturę uzupełniania
        free(extension);

        return true;
    }
}

bool removeRoad(Map *map, const char *city1, const char *city2) {
    if (map == NULL) {
        return false;
    }
    else if (!correctName(city1) || !correctName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    else {
        RoadSeg* targetRoad;
        bool noError;

        targetRoad = findRoad(map, city1, city2);
        if (targetRoad == NULL) {
            return false;
        }

        if (!(targetRoad->belongsToRoute)) {
            onlyRemoveRoad(map, targetRoad);
            return true;
        }

        noError = fillContainingRoutes(targetRoad, map->firstRoute);
        if (!noError) {
            return false;
        }
        else {
            onlyRemoveRoad(map, targetRoad);
            return true;
        }
    }
}

char const* getRouteDescription(Map *map, unsigned routeId) {
    if (map == NULL || routeId < MIN_ROUTE_ID || routeId > MAX_ROUTE_ID) {
        return emptyString();
    }
    else {
        WholeRoute* route;

        route = findRoute(map, routeId);
        if (route == NULL)
            return emptyString();

        return toString(route);
    }
}

/** @brief Cofa zmiany dokonane na mapie
 * W przypadku błędu, wywoływana jest, by wycofać dodanie dróg lub miast
 * oraz zmiany ostatnich dat remontów.
 *
 * @param map              - wskaźnik na mapę;
 * @param stack            - wskaźnik na stos;
 * @param citiesAdded      - liczba dodanych miast;
 * @param roadsAdded       - liczba dodanych dróg;
 */
static void reverseAllChanges(Map *map, RouteStack* stack, size_t citiesAdded,
                              size_t roadsAdded) {
    RoadSeg* currentSeg;
    City* currentCity;

    while (!empty(stack)) {
        currentSeg = pop(stack);
        currentSeg->lastRepairYear = currentSeg->previousRepairYear;
    }

    currentSeg = map->firstRoad;

    for (size_t i = 0; i < roadsAdded; i++) {
        onlyRemoveRoad(map, currentSeg);
        currentSeg = map->firstRoad;
    }

    currentCity = map->firstCity;

    for (size_t i = 0; i < citiesAdded; i++) {
        map->firstCity = currentCity->nextCity;
        free(currentCity->name);
        free(currentCity->roads);
        free(currentCity);
        currentCity = map->firstCity;
    }
}

/** @brief Wykańcza proces tworzenia drogi z opisu
 * Sprawdza, czy tworzenie się powiodło i, jeśli tak,
 * wykańcza proces tworzenia drogi z opisu.
 *
 * @param error             - informacja o błędzie;
 * @param map               - wskaźnik na mapę;
 * @param citiesAdded       - liczba dodanych miast;
 * @param roadsAdded        - liczba dodanych dróg;
 * @param stack             - wskaźnik na stos;
 * @param city1Name         - napis z nazwą miasta 1;
 * @param city2El           - wskaźnik na strukturę z nazwą ostatniego miasta;
 * @param routeId           - id drogi;
 * @return Wartość @p true, jeśli wszystko się udało, @p false, jeśli wystąpił błąd.
 */
static bool finishRouteFromDescription(bool error, Map* map, size_t citiesAdded,
                                       size_t roadsAdded, RouteStack* stack,
                                       char* city1Name, CmdElement* city2El,
                                       unsigned routeId) {
    WholeRoute* newRoute;
    City *city1Struc, *city2Struc;

    if (error) {
        reverseAllChanges(map, stack, citiesAdded, roadsAdded);
        free(stack);
        return false;
    }
    else {
        reverseStack(stack);

        newRoute = transform(stack);
        if (newRoute == NULL) {
            reverseAllChanges(map, stack, citiesAdded, roadsAdded);
            free(stack);
            return false;
        }

        recognizeCities(stack, &city1Struc, &city2Struc,
                        city1Name, city2El->input);

        finalizeRoute(map, newRoute, routeId, city1Struc, city2Struc);
        free(stack);

        return true;
    }
}

bool newRouteFromDescription(Map *map, unsigned routeId, CmdElement* cmd) {
    if (map == NULL || routeId < MIN_ROUTE_ID || routeId > MAX_ROUTE_ID) {
        return false;
    }
    else if (findRoute(map, routeId) != NULL) {
        return false;
    }
    else {
        RouteStack* stack;
        RoadSeg* current;
        CmdElement *city1El, *city2El;

        unsigned lengthVal;
        int yearVal;
        size_t citiesAdded, roadsAdded;
        bool error;
        char* city1Name;

        stack = createStack(NULL, NULL);
        if (stack == NULL)
            return false;

        citiesAdded = 0;
        roadsAdded = 0;
        error = false;

        city1El = cmd->nextElement;
        city2El = NULL;
        city1Name = city1El->input;

        // Funkcję newRouteFromDescription wywołujemy tylko, gdy wiemy,
        // że polecenie wpisane wcześniej na wejście ma przynajmniej prawidłową
        // strukturę, więc ta pętla wykona się co najmniej raz.
        while (city1El->nextElement != NULL && !error) {
            translateRoadDescription(city1El, &city2El, &lengthVal, &yearVal);

            current = addOrRepairRoad(map, city1El->input, city2El->input,
                                      lengthVal, yearVal, &citiesAdded,
                                      &roadsAdded);
            if (current == NULL) {
                error = true;
            }
            else {
                push(stack, current);

                city1El = city2El;
            }
        }

        return finishRouteFromDescription(error, map, citiesAdded, roadsAdded,
                                          stack, city1Name, city2El, routeId);
    }
}

bool removeRoute(Map* map, unsigned routeId) {
    if (map == NULL || routeId < MIN_ROUTE_ID || routeId > MAX_ROUTE_ID) {
        return false;
    }
    else {
        WholeRoute *previous, *route;

        previous = scrollRoutes(map, routeId);
        // Jeśli nie istnieje droga o podanym numerze.
        if (previous == NULL)
            return false;

        if (previous == map->firstRoute && previous->routeId == routeId) {
            route = previous;
            map->firstRoute = route->nextWholeRoute;
        }
        else {
            route = previous->nextWholeRoute;
            previous->nextWholeRoute = route->nextWholeRoute;
        }

        tidySharedSegments(route, map);

        deleteWholeRoute(route);

        return true;
    }
}