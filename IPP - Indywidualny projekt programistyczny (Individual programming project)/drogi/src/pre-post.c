/** @file
 * Implementacje funkcji potrzebnych do przygotowania programu do działania
 * oraz do przygotowania go do zakończenia.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "pre-post.h"
#include "map.h"
#include "translators.h"
#include "delete.h"

#include <stdbool.h>
#include <limits.h>

bool pre(Map** map, MaxNumber** maxU, MaxNumber** maxInt) {
    *map = newMap();
    if (*map == NULL)
        return false;

    *maxU = prepareMaxNumber(UINT_MAX);
    if (*maxU == NULL) {
        free(*map);
        return false;
    }

    *maxInt = prepareMaxNumber(INT_MAX);
    if (*maxInt == NULL) {
        free(*map);
        free(*maxU);
        return false;
    }

    return true;
}

void post(Map* map, MaxNumber* maxU, MaxNumber* maxInt) {
    deleteMap(map);

    free(maxU->digits);
    free(maxU);

    free(maxInt->digits);
    free(maxInt);
}
