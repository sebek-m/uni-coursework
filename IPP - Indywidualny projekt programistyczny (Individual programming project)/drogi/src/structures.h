/** @file
 * Podstawowe struktury potrzebne do obsługi map.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_STRUCTURES_H
#define DROGI_STRUCTURES_H

#include <stdbool.h>

/**
 * Struktura przechowująca miasto.
 */
typedef struct City City;

/**
 * Struktura przechowująca odcinek drogi.
 */
typedef struct RoadSeg RoadSeg;

/**
 * Struktura przechowująca element listy odcinków,
 * będącej drogą krajową.
 */
typedef struct Route Route;

/**
 * Struktura przechowująca drogę krajową.
 */
typedef struct WholeRoute WholeRoute;

/**
 * Struktura przechowująca miasto.
 */
struct City {
    char* name; /**< Wskaźnik na napis, będący nazwą miasta */
    RoadSeg** roads; /**< Tablica podpiętych odcinków drogi */
    unsigned roadsNum; /**< Liczba podpiętych odcinków drogi */
    unsigned roadsSize; /**< Rozmiar tablicy podpiętych odcinków drogi */
    City* nextCity; /**< Wskaźnik na następne miasto w liście wszystkich miast */
    bool visited; /**< Flaga, która informuje czy miasto zostało odwiedzone */
};

/**
 * Struktura przechowująca odcinek drogi.
 */
struct RoadSeg {
    City *city1; /**< Wskaźnik na miasto podłączone do drogi */
    City *city2; /**< Wskaźnik na miasto podłoączone do drogi */
    unsigned length; /**< Długość odcinka drogi */
    int previousRepairYear; /**< Rok remontu o jeden wcześniejszego niż ostatni */
    int lastRepairYear; /**< Rok ostatniego remontu */
    RoadSeg* nextRoadSeg; /**< Wskaźnik na następny odcinek drogi
                            * w liście wszystkich odcinków */
    bool belongsToRoute; /**< Flaga, która informuje czy odcinek
                           * należy do jakiejś drogi krajowej */
};

/**
 * Struktura przechowująca element listy odcinków,
 * będącej drogą krajową.
 */
struct Route {
    RoadSeg* roadSeg; /**< Wskaźnik na odcinek drogi, któremu
                        * odpowiada ta część drogi krajowej */
    Route* nextSegment; /**< Następny element listy reprezentującej
                          * drogę krajową */
};

/**
 * Struktura przechowująca drogę krajową.
 */
struct WholeRoute {
    Route *first; /**< Wskaźnik na pierwszy element
                    * listy odcinków należących do drogi krajowej */
    Route *last; /**< Wskaźnik na ostatni element
                   * listy odcinków należących do drogi krajowej */
    City *city1; /**< Wskaźnik na pierwsze miasto,
                   * przez które przechodzi droga krajowa */
    City *city2; /**< Wskaźnik na ostatnie miasto,
                   * przez które przechodzi droga krajowa */
    unsigned routeId; /**< Numer drogi krajowej#routeId */
    WholeRoute* nextWholeRoute; /**< Wskaźnik na następny element
                                  * listy wszystkich dróg krajowych */
};

#endif //DROGI_STRUCTURES_H
