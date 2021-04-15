/* Modu³ structure zawiera funkcje potrzebne
 * do obs³ugi struktury drzewa z dopuszczalnymi
 * historiami. */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "structure.h"

// Funkcja alokuje pamiêæ na nowy wêze³,
// czyli na now¹ historiê i standardowo ustawia
// wszystkie pola poza stanem kwantowym na NULL'e.
//  state - znak odpowiadaj¹cy stanowi kwantowemu
History newNode(char state) {
    History newState;

    newState = (History)malloc(sizeof(struct State));
    // Sprawdzamy, czy alokacja siê powiod³a.
    if (!newState)
        exit(1);

    newState->value = state;
    newState->energy = NULL;
    newState->next = NULL;
    newState->prev = NULL;
    newState->zero = NULL;
    newState->one = NULL;
    newState->two = NULL;
    newState->three = NULL;

    return newState;
}
