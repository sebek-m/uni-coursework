/** @file
 * Implementacje funkcji do implementacji funkcji addRoad.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "add-road.h"
#include "utils.h"
#include "structures.h"

#include <stdlib.h>
#include <string.h>

/**
 * Początkowy rozmiar tablicy
 */
#define INITIAL_ARRAY_SIZE 3

/** @brief Wstawia podane miasto do mapy
 * Wstawia podane miasto do listy wszystkich miast.
 *
 * @param[in] map       - wskaźnik na strukturę mapy;
 * @param[in] city      - miasto, które ma zostać podłączone;
 */
static void addCityToMap(Map *map, City* city) {
    city->nextCity = map->firstCity;
    map->firstCity = city;
}

/** @brief Tworzy nowe miasto
 * Tworzy nową instancję struktury reprezentującej miasto.
 *
 * @param[in] city      - wskaźnik na napis, będący nazwą miasta;
 * @return wskaźnik na nową strukturę miasta/NULL w przypadku
 * błędu alokacji
 */
static City* newCity(const char *city) {
    City* newCity;

    newCity = (City*)malloc(sizeof(City));

    if (newCity) {
        newCity->name = (char*)malloc((strlen(city) + 1) * sizeof(char));
        if (newCity->name == NULL) {
            free(newCity);
            return NULL;
        }

        strcpy(newCity->name, city);

        newCity->nextCity = NULL;
        newCity->visited = false;

        newCity->roadsNum = 0;
        // Nowe miasto dodajemy tylko w sytuacji, gdy chcemy do niego podłączyć
        // zaraz odcinek drogi, więc od razu alokujemy pamięć na tablicę odcinków
        // drogi
        newCity->roadsSize = INITIAL_ARRAY_SIZE;
        newCity->roads = (RoadSeg**)calloc(INITIAL_ARRAY_SIZE, sizeof(RoadSeg*));

        if (newCity->roads == NULL) {
            free(newCity);
            return NULL;
        }
    }

    return newCity;
}

/** @brief Tworzy odpowiednie miasta, jeśli nie ma ich jeszcze na mapie.
 * Tworzy odpowiednie miasta, jeśli nie ma ich jeszcze na mapie.
 *
 * @param[in] map               - wskaźnik na strukturę mapy;
 * @param[in, out] city1        - wskaźnik na wskaźnik na miasto, które być może
 *                                ma zostać stworzone
 * @param[in] city1Name         - wskaźnik na napis, będący nazwą miasta
 * @param[in, out] city2        - wskaźnik na wskaźnik na miasto, które być może
 *                                ma zostać stworzone
 * @param[in] city2Name         - wskaźnik na napis, będący nazwą miasta
 * @param[in, out] city1Created - wskaźnik na zmienną, informującą czy miasto
 *                                musiało być stworzone
 * @param[in, out] city2Created - wskaźnik na zmienną, informującą czy miasto
 *                                musiało być stworzone
 * @return Wartość @p true, jeśli nie wystąpił żaden błąd, @p false, jeśli
 * nie udała się jakaś alokacja w trakcie
 */
static bool maybeCreateCities(Map* map, City** city1, const char *city1Name,
                              City** city2, const char *city2Name,
                              bool* city1Created, bool* city2Created) {
    if (*city1 == NULL) {
        *city1 = newCity(city1Name);
        if (*city1) {
            addCityToMap(map, *city1);
            *city1Created = true;
        }
        else {
            return false;
        }
    }
    if (*city2 == NULL) {
        *city2 = newCity(city2Name);
        if (*city2) {
            addCityToMap(map, *city2);
            *city2Created = true;
        }
        else {
            if (*city1Created) {
                map->firstCity = (*city1)->nextCity;
                free((*city1)->roads);
                free((*city1)->name);
                free(*city1);
            }

            return false;
        }
    }

    return true;
}

/** @brief Sprawdza, czy podane miasta są połączone
 * Sprawdza, czy podane miasta są ze sobą połączone odcinkiem drogi.
 * Jeśli tak jest, to zwraca odcinek drogi, który je łączy.
 *
 * @param[in] city1     - wskaźnik na pierwsze miasto
 * @param[in] city2     - wskaźnik na drugie miasto
 * @return Adres do odcinka drogi, łączącego miasta lub NULL jeśli
 * nie są połączone.
 */
static RoadSeg* citiesConnected(City* city1, City* city2) {
    if (city1->roadsNum > city2->roadsNum)
        return citiesConnected(city2, city1);

    RoadSeg **roads, *targetRoad;

    roads = city1->roads;
    targetRoad = NULL;

    for (unsigned i = 0; i < city1->roadsNum && !targetRoad; i++) {
        if (roads[i]->city2 == city2 || roads[i]->city1 == city2)
            targetRoad = roads[i];
    }

    return targetRoad;
}

/** @brief Zwiększa tablicę dróg danego miasta
 * Zwiększa rozmiar pamięci zaalokowanej na tablicę
 * dróg podłączonych do danego miasta
 *
 * @param[in] city      - wskaźnik na miasto;
 * @return Wartość @p true, jeśli alokacja się powiodła,
 * @p false jeśli nie.
 */
static bool expandRoadsArray(City* city) {
    bool success;

    city->roadsSize = 1 + city->roadsSize * 3 / 2;

    RoadSeg** tmpRoads = (RoadSeg**)realloc(city->roads,
                                            city->roadsSize * sizeof(RoadSeg*));
    success = tmpRoads != NULL;
    if (success)
        city->roads = tmpRoads;

    return success;
}

/** @brief Dodaje odcinek drogi do miasta
 * Wstawia dany odcinek drogi do tablicy odcinków drogi
 * danego miasta
 *
 * @param[in] city      - wskaźnik na miasto, do którego podpinana jest droga
 * @param[in] road      - wskaźnik na odcinek drogi, podpinanej do miasta
 * @return Wartość @p true, jeśli ewentualna alokacja pamięci się udała,
 * @p false, jeśli nie.
 */
static bool addRoadToCity(City* city, RoadSeg* road) {
    bool success;

    success = true;

    if (city->roadsNum == city->roadsSize)
        success = expandRoadsArray(city);

    // Jeśli nie udało się zaalkować pamięci na większą
    // tablicę dróg, to nie chcemy ustawiać pola tablicy
    // dróg obsługiwanego miasta na NULL.
    if (success) {
        (city->roads)[city->roadsNum] = road;
        (city->roadsNum)++;
    }

    return success;
}

/** @brief Łączy dwa miasta zadaną drogą
 * Podpina podaną drogę do jednego i drugiego podanego miasta.
 *
 * @param[in] city1         - wskaźnik na miasto, do którego podpinana jest droga;
 * @param[in] city2         - wskaźnik na miasto, do którego podpinana jest droga;
 * @param[in] road          - wskaźnik na odcinek drogi, który ma zostać podpięty
 * @return Wartość @p true, jeśli łączenie się udało, @p false jeśli nie udała się
 * jakaś alokacja pamięci
 */
static bool connectCities(City* city1, City* city2, RoadSeg* road) {
    // Sprawdzamy, czy, jeśli podjęto w addRoadToCity
    // taką próbę, powiększenie tablicy dróg się udało
    if (!addRoadToCity(city1, road)) {
        return false;
    }
    if (!addRoadToCity(city2, road)) {
        (city1->roads)[city1->roadsNum - 1] = NULL;
        (city1->roadsNum)--;
        return false;
    }

    return true;
}

/** @brief Tworzy nowy odcinek drogi
 * Tworzy nową instancję struktury odcinka drogi
 *
 * @param[in] city1         - wskaźnik na miasto, będące jednym końcem drogi;
 * @param[in] city2         - wskaźnik na miasto, będące drugim końcem drogi;
 * @param[in] length        - długość drogi
 * @param[in] builtYear     - rok budowy drogi
 * @return Wskaźnik na nowy odcinek drogi lub NULL jeśli
 * nie powiodła się jakaś alokacja
 */
static RoadSeg* createRoad(City* city1, City* city2,
                           unsigned length, int builtYear) {
    RoadSeg* newRoad;

    newRoad = (RoadSeg*)malloc(sizeof(RoadSeg));

    if (newRoad) {
        newRoad->city1 = city1;
        newRoad->city2 = city2;

        if (!connectCities(city1, city2, newRoad)) {
            free(newRoad);
            return NULL;
        }

        newRoad->length = length;
        newRoad->lastRepairYear = builtYear;
        newRoad->previousRepairYear = builtYear;

        newRoad->nextRoadSeg = NULL;
        newRoad->belongsToRoute = false;
    }

    return newRoad;
}

/** @brief Dodaje odcinek drogi do mapy
 * Wstawia odcinek drogi do listy wszystkich odcinków
 *
 * @param[in] map       - wskaźnik na strukturę mapy;
 * @param[in] road      - wskaźnik na wstawiany odcinek;
 */
static void addRoadToMap(Map* map, RoadSeg* road) {
    road->nextRoadSeg = map->firstRoad;
    map->firstRoad = road;
}

/** @brief Usuwa miasta, które zostały stworzone zbyt pochopnie.
 * Usuwa miasta, które zostały stworzone pomimo braku możliwości
 * wykonania pewnej innej rzeczy w funkcji addOrRepairRoad.
 *
 * @param map                 - wskaźnik na mapę;
 * @param city1               - wskaźnik na miasto 1;
 * @param city2               - wskaźnik na miasto 1;
 * @param city1Created        - informacja, czy miasto 1 było tworzone;
 * @param city2Created        - informacja, czy miasto 1 było tworzone;
 */
static void reverseCityCreation(Map* map, City* city1, City* city2,
                                bool city1Created, bool city2Created) {
    if (city2Created) {
        map->firstCity = city2->nextCity;
        free(city2->roads);
        free(city2->name);
        free(city2);
    }
    if (city1Created) {
        map->firstCity = city1->nextCity;
        free(city1->roads);
        free(city1->name);
        free(city1);
    }
}

bool initializeRoad(Map *map, const char *city1, const char *city2,
                    unsigned length, int builtYear) {
    City *city1Struc, *city2Struc;
    RoadSeg* newRoad;
    bool city1Created, city2Created;

    city1Created = false;
    city2Created = false;

    city1Struc = findCity(map, city1);
    city2Struc = findCity(map, city2);

    // Jeśli jakaś alokacja pamięci się nie powiodła
    if (!maybeCreateCities(map, &city1Struc, city1, &city2Struc, city2,
                           &city1Created, &city2Created)) {
        return false;
    }

    // Jeśli miasta już połączone
    if (!city1Created && !city2Created &&
        citiesConnected(city1Struc, city2Struc) != NULL) {
        return false;
    }

    newRoad = createRoad(city1Struc, city2Struc, length, builtYear);
    // Jeśli jakaś alokacja pamięci się nie powiodła
    if (newRoad == NULL) {
        reverseCityCreation(map, city1Struc, city2Struc,
                            city1Created, city2Created);

        return false;
    }

    addRoadToMap(map, newRoad);

    return true;
}

/** @brief Sprawdza, czy parametry oczekiwanej drogi są poprawne.
 * Sprawdza, czy rok i długość podane przez użytkownika są zgodne
 * z aktualnym rokiem ostatniego remontu i długością istniejącego odcinka drogi.
 *
 * @param road             - wskaźnik na odcinek;
 * @param length           - oczekiwana długość;
 * @param year             - oczekiwany rok ostatniego remontu;
 * @return Wartość @p true, jeśli parametry są niepoprawne, @p false jeśli poprawne.
 */
static bool invalidRoadParameters(RoadSeg* road, unsigned length, int year) {
    return year < road->lastRepairYear || length != road->length;
}

/** @brief Próba remontu istniejącej drogi
 * Funkcja sprawdza, czy nowe parametry drogi są dopuszczalne
 * i, jeśli tak, modyfikuje rok ostatniego remontu.
 *
 * @param road           - wskaźnik na drogę;
 * @param length         - oczekiwana długość;
 * @param year           - oczekiwany rok ostatniego remontu;
 * @return Wskaźnik na wyremontowaną drogę, NULL w przypadku błędu.
 */
static RoadSeg* repairedRoad(RoadSeg* road, unsigned length, int year) {
    if (invalidRoadParameters(road, length, year))
        return NULL;

    road->previousRepairYear = road->lastRepairYear;
    road->lastRepairYear = year;

    return road;
}

/** Dokańcza proces dodawania lub remontu drogi
 * Dodaje droge do mapy oraz aktualizuje zmienne
 * informujące o ilośći dodanych miast i dróg.
 *
 * @param map                 - wskaźnik na mapę.
 * @param road                - wskaźnik na odcinek drogi;
 * @param citiesAdded         - wskaźnik na zmienną zliczającą miasta;
 * @param roadsAdded          - wskaźnik na zmienną zliczającą drogi;
 * @param city1Created        - informacja czy dodano miasto 1;
 * @param city2Created        - informacja czy dodano miasto 2;
 */
static void finalizeRoad(Map* map, RoadSeg* road, size_t* citiesAdded,
                         size_t* roadsAdded, bool city1Created, bool city2Created) {
    addRoadToMap(map, road);
    (*roadsAdded)++;

    if (city1Created)
        (*citiesAdded)++;
    if (city2Created)
        (*citiesAdded)++;
}

RoadSeg* addOrRepairRoad(Map *map, const char *city1, const char *city2,
                         unsigned length, int year, size_t* citiesAdded,
                         size_t* roadsAdded) {
    City *city1Struc, *city2Struc;
    RoadSeg* road;
    bool city1Created, city2Created;

    city1Created = false;
    city2Created = false;
    city1Struc = findCity(map, city1);
    city2Struc = findCity(map, city2);

    // Jeśli jakaś alokacja pamięci się nie powiodła
    if (!maybeCreateCities(map, &city1Struc, city1, &city2Struc, city2,
                           &city1Created, &city2Created)) {
        return NULL;
    }

    // Jeśli miasta już istnieją i są połączone, wykonywana jest próba remontu
    if (!city1Created && !city2Created) {
        road = citiesConnected(city1Struc, city2Struc);
        if (road != NULL)
            return repairedRoad(road, length, year);
    }

    // Jeśli któreś z miast nie istniało, lub nie są one połączone
    road = createRoad(city1Struc, city2Struc, length, year);
    // Jeśli jakaś alokacja pamięci się nie powiodła
    if (road == NULL) {
        reverseCityCreation(map, city1Struc, city2Struc,
                            city1Created, city2Created);

        return NULL;
    }

    finalizeRoad(map, road, citiesAdded, roadsAdded,
                 city1Created, city2Created);

    return road;
}