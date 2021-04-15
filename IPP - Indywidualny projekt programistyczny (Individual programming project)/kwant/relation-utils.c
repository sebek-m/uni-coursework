/* Modu� relation-utils zawiera funkcje u�ytkowe
 * wymagane w obs�udze relacji r�wnowa�no�ci energii.
 * U�ywane s� one w modu�ach equal oraz remove. */

#include "relation-utils.h"
#include "structure.h"

// Funkcja pod��cza do siebie nawzajem dane elementy listy
// dwukierunkowej. Bierze udzia� w scalaniu dw�ch klas
// r�wnowa�no�ci energii przypisanych historiom, co dzieje
// si� w wyniku prawid�owego wywo�ania komendy EQUAL.
// W�ze� history1 to element listy kt�ry w nowym ustawieniu
// ma w tej li�cie poprzedza� w�ze� history2
void connectEnds(History history1, History history2) {
    // Sprawdzamy czy nie s� NULL'ami, poniewa� mo�e zda�y� si�,
    // �e chcemy podpi�� list� do ko�c�wki drugiej
    if (history2)
        history2->prev = history1;

    if (history1)
        history1->next = history2;
}
