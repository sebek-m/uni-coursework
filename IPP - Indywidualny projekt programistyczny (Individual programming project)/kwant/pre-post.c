/* Modu³ pre-post zawiera funkcje do przygotowania potrzebnych informacji
 * przed rozpoczêciem symulacji oraz do wyczyszczenia pamiêci po jej
 * zakoñczeniu. Wywo³ywane s¹ one w module main. */

#include <stdlib.h>
#include <inttypes.h>
#include "pre-post.h"
#include "structure.h"

// Funkcja tworzy tablicê, w której zapisany jest ci¹g integer’ów,
// bêd¹cych cyframi liczby 2^64 - 1, czyli maksymalnej akceptowalnej energii.
// Tablica ta s³u¿y do póŸniejszej referencji w przypadku wywo³ania
// komendy "ENERGY"
//  maxEnergyArray  - wskaŸnik na wczeœniej zadeklarowan¹ zmienn¹ wskaŸnikow¹
//                    typu int (czyli na tworzon¹ tablicê)
//  maxEnergyLength - wskaŸnik na zmienn¹ odpowiadaj¹c¹ d³ugoœci
//                  - tworzonej tablicy
void prepareMaxEnergyArray(int** maxEnergyArray, int* maxEnergyLength) {
    uint64_t maxEnergy;

    // Korzystamy z macro reprezentuj¹cego liczbê 2^64 - 1
    // (z biblioteki inttypes.g)
    maxEnergy = UINT64_MAX;
    *maxEnergyLength = 0;

    // Zliczamy liczbê cyfr
    while (maxEnergy != 0) {
        maxEnergy /= 10;
        (*maxEnergyLength)++;
    }

    *maxEnergyArray = (int*)malloc((*maxEnergyLength) * sizeof(int));
    if (!(*maxEnergyArray))
        exit(1);

    maxEnergy = UINT64_MAX;

    // Poprzez dzielenie 2^64 - 1 w pêtli przez 10 "kasujemy" kolejne
    // cyfry od koñca, by móc je "sczytaæ" poprzez dzia³anie modulo
    for (int i = (*maxEnergyLength) - 1; maxEnergy != 0; maxEnergy /= 10, i--) {
        (*maxEnergyArray)[i] = maxEnergy % 10;
    }
}

// Na skutek bycia wywo³an¹ w funkcji deleteTreeFinal, funkcja ustawia
// pola energy wszystkich wêz³ów, czyli historii, których energie s¹
// w relacji równowa¿noœci z energi¹ historii hisTree na NULL,
// aby w póŸniejszej fazie usuwania drzewa z historiami unikn¹æ podwójnego
// wywo³ania free na tym samym adresie.
//  hisTree - adres wêz³a historii, której energiê bêdziemy usuwaæ
static void deleteEnergies(History hisTree) {
    History current;

    current = hisTree->next;

    while (current) {
        current->energy = NULL;
        current = current->next;
    }

    current = hisTree->prev;

    while (current) {
        current->energy = NULL;
        current = current->prev;
    }

    // Zwalniamy pamiêæ po reprezentacji
    // energii historii bêd¹cych w danej klasie równowa¿noœci
    free(hisTree->energy);
    hisTree->energy = NULL;
}

// Wywo³ywana na koñcu programu, aby rekurencyjnie zwolniæ pamiêæ
// po drzewie z dopuszczalnymi historiami
//  hisTree - korzeñ drzewa z historiami
void deleteTreeFinal(History hisTree) {
    if (hisTree){
        deleteTreeFinal(hisTree->zero);
        deleteTreeFinal(hisTree->one);
        deleteTreeFinal(hisTree->two);
        deleteTreeFinal(hisTree->three);

        if (hisTree->energy != NULL)
            deleteEnergies(hisTree);

        // Samego wêz³a nie musimy NULL'owaæ, poniewa¿ ta funkcja
        // wykonywana jest tu¿ przed zakoñczeniem programu,
        // wiêc nie ma ryzyka natrafienia na wskaŸnik do adresu
        // ze zwolnion¹ pamiêci¹.
        free(hisTree);
    }
}
