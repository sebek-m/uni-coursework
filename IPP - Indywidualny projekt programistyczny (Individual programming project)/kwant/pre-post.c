/* Modu� pre-post zawiera funkcje do przygotowania potrzebnych informacji
 * przed rozpocz�ciem symulacji oraz do wyczyszczenia pami�ci po jej
 * zako�czeniu. Wywo�ywane s� one w module main. */

#include <stdlib.h>
#include <inttypes.h>
#include "pre-post.h"
#include "structure.h"

// Funkcja tworzy tablic�, w kt�rej zapisany jest ci�g integer��w,
// b�d�cych cyframi liczby 2^64 - 1, czyli maksymalnej akceptowalnej energii.
// Tablica ta s�u�y do p�niejszej referencji w przypadku wywo�ania
// komendy "ENERGY"
//  maxEnergyArray  - wska�nik na wcze�niej zadeklarowan� zmienn� wska�nikow�
//                    typu int (czyli na tworzon� tablic�)
//  maxEnergyLength - wska�nik na zmienn� odpowiadaj�c� d�ugo�ci
//                  - tworzonej tablicy
void prepareMaxEnergyArray(int** maxEnergyArray, int* maxEnergyLength) {
    uint64_t maxEnergy;

    // Korzystamy z macro reprezentuj�cego liczb� 2^64 - 1
    // (z biblioteki inttypes.g)
    maxEnergy = UINT64_MAX;
    *maxEnergyLength = 0;

    // Zliczamy liczb� cyfr
    while (maxEnergy != 0) {
        maxEnergy /= 10;
        (*maxEnergyLength)++;
    }

    *maxEnergyArray = (int*)malloc((*maxEnergyLength) * sizeof(int));
    if (!(*maxEnergyArray))
        exit(1);

    maxEnergy = UINT64_MAX;

    // Poprzez dzielenie 2^64 - 1 w p�tli przez 10 "kasujemy" kolejne
    // cyfry od ko�ca, by m�c je "sczyta�" poprzez dzia�anie modulo
    for (int i = (*maxEnergyLength) - 1; maxEnergy != 0; maxEnergy /= 10, i--) {
        (*maxEnergyArray)[i] = maxEnergy % 10;
    }
}

// Na skutek bycia wywo�an� w funkcji deleteTreeFinal, funkcja ustawia
// pola energy wszystkich w�z��w, czyli historii, kt�rych energie s�
// w relacji r�wnowa�no�ci z energi� historii hisTree na NULL,
// aby w p�niejszej fazie usuwania drzewa z historiami unikn�� podw�jnego
// wywo�ania free na tym samym adresie.
//  hisTree - adres w�z�a historii, kt�rej energi� b�dziemy usuwa�
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

    // Zwalniamy pami�� po reprezentacji
    // energii historii b�d�cych w danej klasie r�wnowa�no�ci
    free(hisTree->energy);
    hisTree->energy = NULL;
}

// Wywo�ywana na ko�cu programu, aby rekurencyjnie zwolni� pami��
// po drzewie z dopuszczalnymi historiami
//  hisTree - korze� drzewa z historiami
void deleteTreeFinal(History hisTree) {
    if (hisTree){
        deleteTreeFinal(hisTree->zero);
        deleteTreeFinal(hisTree->one);
        deleteTreeFinal(hisTree->two);
        deleteTreeFinal(hisTree->three);

        if (hisTree->energy != NULL)
            deleteEnergies(hisTree);

        // Samego w�z�a nie musimy NULL'owa�, poniewa� ta funkcja
        // wykonywana jest tu� przed zako�czeniem programu,
        // wi�c nie ma ryzyka natrafienia na wska�nik do adresu
        // ze zwolnion� pami�ci�.
        free(hisTree);
    }
}
