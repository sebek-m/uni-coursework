/** @file
 * Funkcje pomocnicze przydatne na różnych
 * etapach działania programu.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_UTILS_H
#define DROGI_UTILS_H

#include "map.h"
#include "structures.h"

#include <stdbool.h>

/** @brief Znajduje miasto o podanej nazwie.
 * Znajduje wskaźnik na miasto o podanej nazwie.
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city       – wskaźnik na napis reprezentujący nazwę miasta;
 * @return wskaźnik na szukane miasto/NULL w przypadku braku
 * miasta o takiej nazwie
 */
City* findCity(Map* map, const char *city);

/** @brief Sprawdza, czy podany znak jest dopuszczalny
 * Sprawdza, czy podany znak może być częścią prawidłowej
 * nazwy miasta.
 *
 * @param[in] c             - sprawdzany znak
 * @return Wartość @p true, jeśli znak dopuszczalny, @p false w przeciwnym przypadku.
 */
bool validChar(char c);

/** @brief Sprawdza, czy podana nazwa miasta jest prawidłowa
 * Sprawdza, czy podana nazwa miasta jest prawidłowa,
 * czyli czy nie jest pusta i czy nie zawiera niedozwolonych
 * znaków.
 *
 * @param[in] cityName       – wskaźnik na napis reprezentujący nazwę miasta;
 * @return Wartość @p true, jeśli nazwa poprawna, @p false w przeciwnym przypadku.
 */
bool correctName(const char *cityName);

/** @brief Znajduje drogę łączącą zadane miasta.
 * Znajduje drogę łączącą miasta reprezentowane przez ich nazwy,
 * podane jako city1 i city2.
 *
 * NULL jako wynik funkcji oznacza, że city1
 * nie istnieje lub, że odcinek drogi między city1 i city2
 * nie istnieje. Może się zdarzyć, że drugi przypadek jest
 * jednoznaczny z tym, że city2 nie istnieje.
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @return wskaźnik na szukaną drogę
 */
RoadSeg* findRoad(Map *map, const char *city1, const char *city2);

/** @brief Znajduje drogę krajową o podanym numerze
 * Znajduje wskaźnik na drogę krajową o podanym numerze.
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @return Wskaźnik na drogę krajową o podanym numerze/NULL jeśli nie ma
 * takiej drogi.
 */
WholeRoute* findRoute(Map *map, unsigned routeId);

/** @brief Znajduje drogę krajową, poprzedzającą drogę o podanym numerze.
 * Znajduje wskaźnik na drogę krajową, która poprzedza w liście
 * wszystkich dróg drogę o podanym numerze.
 *
 * @param[in] map        – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @return Wskaźnik na drogę krajową poprzedzającą drogę o
 * podanym numerze lub, gdy droga o numerze routeId to pierwsza droga na liście
 * wszystkich dróg, wskaźnik na drogę o podanym numerze. NULL jeśli nie ma drogi
 * o podanym numerze.
 */
WholeRoute* scrollRoutes(Map* map, unsigned routeId);

/** @brief Oznacza wszystkie odcinki w drodze krajowej, jako należące do
 * drogi krajowej.
 * Ustawia pole belongsToRoute każdego odcinka drogi na true.
 *
 * @param[in] route      - wskaźnik na drogę krajową, którą chcemy
 *                         zmodyfikować;
 */
void markAllAsBelonging(WholeRoute* route);

/** @brief Oznacza wszystkie miasta w drodze krajowej jako odwiedzone.
 * Ustawia pole visited na true w każdym mieście podłączonym do
 * drogi będącej częścią drogikrajowej.
 *
 * @param[in] route      - wskaźnik na drogę krajową, którą chcemy
 *                         zmodyfikować;
 */
void markAllAsVisited(WholeRoute* route);

/** @brief Oznacza wszystkie miasta w drodze krajowej jako nieodwiedzone.
 * Ustawia pole visited na false w każdym mieście podłączonym do
 * drogi będącej częścią drogikrajowej.
 *
 * @param[in] route      - wskaźnik na drogę krajową, którą chcemy
 *                         zmodyfikować;
 */
void markAllAsUnvisited(WholeRoute* route);

/** @brief Wyznacza drugie miasto podłączone do tego samego odcinka drogi.
 * Wyznacza miasto, które jest podłączone do odcinka drogi road, ale nie
 * jest miastem otherCity.
 *
 * @param[in] road        - wskaźnik na odcinek drogi;
 * @param[in] otherCity   - wskaźnik namiasto, zamiast
 *                          którego chcemy dostać to dobre;
 * @return wskaźnik na żądane miasto.
 */
City* otherCity(RoadSeg* road, City* otherCity);

/** @brief Odwraca drogę.
 * Odwraca listę reprezentującą drogę.
 *
 * @param[in] route     - wskaźnik na odwracaną drogę;
 */
void reverseRoute(WholeRoute* route);

/** @brief Ustawia pole belongsToRoute każdego odcinka w drodze na zgodne z prawdą.
 * Ustawia pole belongsToRoute każdego odcinka w usuwanej drodze na zgodne z prawdą,
 * na podstawie tego, czy, po usunięciu tej drogi, odcinek nadal należy do jakiejś
 * innej drogi.
 *
 * @param route            - droga, którą usuwamy z mapy;
 * @param map              - wskaźnik na strukturę mapy;
 */
void tidySharedSegments(WholeRoute* route, Map* map);

#endif //DROGI_UTILS_H
