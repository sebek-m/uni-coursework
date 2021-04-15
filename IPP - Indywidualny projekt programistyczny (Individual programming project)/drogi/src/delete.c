/** @file
 * Funkcje służące do usuwania różnych struktur.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "delete.h"

#include <stdlib.h>
#include <stdio.h>

void deleteWholeRoute(WholeRoute* route) {
    Route *current, *next;

    current = route->first;

    while (current != NULL) {
        next = current->nextSegment;

        free(current);

        current = next;
    }

    free(route);
}

void deleteRoads(Map *map) {
    RoadSeg *current, *next;

    current = map->firstRoad;

    while (current != NULL) {
        next = current->nextRoadSeg;

        free(current);

        current = next;
    }
}

void deleteCities(Map* map) {
    City *current, *next;

    current = map->firstCity;

    while (current != NULL) {
        next = current->nextCity;

        free(current->roads);
        free(current->name);
        free(current);

        current = next;
    }
}

void deleteRoutes(Map* map) {
    WholeRoute *current, *next;

    current = map->firstRoute;

    while (current != NULL) {
        next = current->nextWholeRoute;

        deleteWholeRoute(current);

        current = next;
    }
}