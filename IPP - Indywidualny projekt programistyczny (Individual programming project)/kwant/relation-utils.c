/* Modu³ relation-utils zawiera funkcje u¿ytkowe
 * wymagane w obs³udze relacji równowa¿noœci energii.
 * U¿ywane s¹ one w modu³ach equal oraz remove. */

#include "relation-utils.h"
#include "structure.h"

// Funkcja pod³¹cza do siebie nawzajem dane elementy listy
// dwukierunkowej. Bierze udzia³ w scalaniu dwóch klas
// równowa¿noœci energii przypisanych historiom, co dzieje
// siê w wyniku prawid³owego wywo³ania komendy EQUAL.
// Wêze³ history1 to element listy który w nowym ustawieniu
// ma w tej liœcie poprzedzaæ wêze³ history2
void connectEnds(History history1, History history2) {
    // Sprawdzamy czy nie s¹ NULL'ami, poniewa¿ mo¿e zda¿yæ siê,
    // ¿e chcemy podpi¹æ listê do koñcówki drugiej
    if (history2)
        history2->prev = history1;

    if (history1)
        history1->next = history2;
}
