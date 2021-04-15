/** @file
 * Funkcje potrzebne do implementacji funkcji getRouteDescription.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#ifndef DROGI_ROUTE_DESCRIPTION_H
#define DROGI_ROUTE_DESCRIPTION_H

#include "structures.h"

/** @brief Zwraca pusty napis.
 * Alokuje pamięc na pusty napis i zwraca na niego adres.
 *
 * @return Adres na pusty napis.
 */
char const* emptyString();

/** @brief Tworzy opis drogi krajowej.
 * Alokuje pamięć na opis drogi krajowej i go tworzy.
 *
 * @param[in] route     - opisywana droga krajowa;
 * @return Wskaźnik na napis, będący opisem drogi, NULL
 * jeśli jakaś alokacja pamięci się nie udała.
 */
char const* toString(WholeRoute* route);

#endif //DROGI_ROUTE_DESCRIPTION_H
