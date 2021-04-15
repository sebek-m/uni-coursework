/* Modu³ energy zawiera funkcje potrzebne
 * do obs³ugi komendy ENERGY. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "energy.h"
#include "utils.h"
#include "tree-search.h"
#include "structure.h"

#define INITIAL_ARRAY_SIZE 5

// Funkcja sprawdza czy znak podany jako
// input jest cyfr¹ (tzn. czy jego kod ASCII
// jest z zakresu 0-9).
static bool inputIsDigit(char input) {
    return (input >= '0' && input <= '9');
}

// Funkcja sprawdza, czy w aktualnie wype³nianej tablicy jest miejsce
// i w przeciwnym razie zwiêksza przypisan¹ jej pamiêæ. Nastêpnie wpisuje
// ostatnio pobran¹ z wejœcia cyfrê do pierwszej wolnej komórki
// oraz pobiera kolejn¹ cyfrê.
//  array   - wskaŸnik do wype³nianej tablicy integer'ów
//  c       - wskaŸnik na adres ostatnio wczytanego znaku
//  i       - wskaŸnik na adres aktualnego indeksu
//  size    - wskaŸnik na adres zmiennej informuj¹cej o rozmiarze aktualnie
//            przypisanej pamieci
static void addDigitToArray(int** array, char* c, int* i, int* size) {
    // Alokujemy trochê wiêcej pamiêci ni¿ jest dok³adnie potrzebne,
    // ¿eby zmniejszyæ liczbê wywo³añ drogiej procedury alokacji pamiêci.
    if (*i == *size) {
        *size = 1 + (*size) * 3 / 2;

        *array = (int*)realloc(*array, (*size) * sizeof(int));
        // Sprawdzamy, czy pamiêæ zosta³a poprawnie zwiêkszona
        if (!(*array))
            exit(1);
    }

    // Kod ASCII to integer wiêc, po odjêciu od chara c, który
    // reprezentuje cyfrê, kodu chara '0', równego 48,
    // dostaniemy kod ASCII równy cyfrze ze zmiennej c
    (*array)[*i] = (*c) - '0';
    (*i)++;
    *c = getchar();
}

// Funkcja czyta z wejœcia znaki, które potencjalnie
// reprezentuj¹ cyfry i wpisuje je kolejno do tablicy
// z dynamicznie alokowan¹ pamiecia,
// tym samym tworz¹c tymczasow¹ reprezezentacjê podanej
// na wejœcie energii.
// tmpEnergy        - wskaŸnik na tablicê, w które ma byæ
//                    przechowywana energia
// tmpEnergyLength  - wskaŸnik na zmienn¹ przechowuj¹c¹ rozmiar
//                    tworzonej tablicy
// c                - wskaŸnjk na wczeœniej zadeklarowan¹ zmienn¹,
//                    która przechowywaæ bêdzie ostatnio wczytany znak
static void prepareTemporaryEnergyArray(int** tmpEnergy, int* tmpEnergyLength, char* c) {
    int size;

    *c = getchar();
    size = INITIAL_ARRAY_SIZE;

    *tmpEnergy = (int*)malloc(size * sizeof(int));
    // Sprawdzamy czy alokacja pamiêci siê powiod³a
    if (!(*tmpEnergy))
        exit(1);

    *tmpEnergyLength = 0;

    // Pêtla zatrzyma siê, gdy zostanie napotkany znak niebêd¹cy cyfr¹.
    while (inputIsDigit(*c))
        addDigitToArray(tmpEnergy, c, tmpEnergyLength, &size);
}

// Funkcja sprawdza, czy energia, wczeœniej podana na wejœcie,
// mieœci siê w zakresie liczb ca³kowitych [1, 2^64 - 1]
//  tmpEnergy       - tablica z cyframi energii z wejœcia
//  tmpEnergyLength - rozmiar tej tablicy
//  maxEnergyArray  - tablica z cyframi liczby 2^64 - 1
//  maxEnergyLength - rozmiar tej tablicy
static bool validEnergy(int* tmpEnergy, int tmpEnergyLength, int* maxEnergyArray, int maxEnergyLength) {
    bool valid;

    // Sprawdzamy czy podano jak¹kolwiek energiê, czy jej
    // reprezentacja ma niewiêcej cyfr ni¿ maksymalna energia
    // oraz czy nie wpisano wiod¹cych zer
    valid = (tmpEnergyLength > 0 &&
             tmpEnergyLength <= maxEnergyLength &&
             tmpEnergy[0] != 0);

    // Podan¹ energiê chcemy porównaæ z maksymaln¹ energi¹ cyfra
    // po cyfrze tylko wtedy, gdy maj¹ tyle samo cyfr,
    // poniewa¿ jest to jedyna sytuacja,
    // kiedy ró¿nica miêdzy ich wielkoœciami jest niepewna
    if (tmpEnergyLength == maxEnergyLength)
        for (int i = 0; i < maxEnergyLength && valid; i++)
            valid = (tmpEnergy[i] <= maxEnergyArray[i]);

    return valid;
}

// Funkcja zwraca liczbê typu uint64_t, której cyfry
// przechowywane s¹ w tablicy
//  array       - tablica z tymi cyframi
//  arrayLength - rozmiar tej tablicy
static uint64_t convertArrayToUint(int* array, int arrayLength) {
    uint64_t value, multiplier;

    value = 0;
    multiplier = 1;

    // Integery z tablicy s¹ tu niejawnie konwertowane na
    // typ uint64_t
    for (int i = arrayLength - 1; i >= 0; multiplier *= 10, i--) {
        value += array[i] * multiplier;
    }

    return value;
}

// Funkcja wypisuje na standardowe wyjœcie
// energiê danej historii, jeœli posiada ona
// przypisan¹ energiê. W przeciwnym przypadku
// wypisuje ERROR na stderr
//  hisTree - adres do wêz³a odpowiadaj¹cemu historii,
//            której energiê chcemy sprawdziæ
static void checkEnergy(History hisTree) {
    if (hisTree->energy != NULL) {
        printf("%"PRIu64"\n", *(hisTree->energy));
    }
    else {
        error();
    }
}

// Jeœli dana historia ma ju¿ przypisan¹ energiê, to funkcja zmienia wartoœæ
// tej energii, a w przeciwnym wypadku przypisuje jej now¹ energiê,
// wczeœniej alokuj¹c pamiêæ na tê energiê
//  targetHistory   - historia, której chcemy przypisaæ energiê
//  energy          - tablica z cyframi liczby odpowiadaj¹cej energii,
//                    któr¹ chcemy przypisaæ
//  energyLength    - rozmiar tej tablicy
static void setEnergy(History targetHistory, int* energy, int energyLength) {
    if (targetHistory->energy != NULL) {
        *(targetHistory->energy) = convertArrayToUint(energy, energyLength);
    }
    else {
        targetHistory->energy = (uint64_t*)malloc(sizeof(uint64_t));
        // Sprawdzamy czy alokacja siê powiod³a
        if (!(targetHistory->energy))
            exit(1);

        *(targetHistory->energy) = convertArrayToUint(energy, energyLength);
    }

    printf("OK\n");
}

// Procedura wykonywana w przypadku, gdy do komendy ENERGY podano dwa argumenty
//  targetHistory   - tablica z reprezentacj¹ wczeœniej podanej historii
//  maxEnergyArray  - tablica z cyframi maksymalnej energii
//  maxEnergyLength - rozmiar tej tablicy
//  c               - wskaŸnik na zmienn¹ ze znakiem ostatnio
//                    przeczytanym z wejœcia
static void newEnergyProcedure(History targetHistory, int* maxEnergyArray,
                               int maxEnergyLength, char* c) {
    int *tmpEnergy, tmpEnergyLength;

    prepareTemporaryEnergyArray(&tmpEnergy, &tmpEnergyLength, c);

    // Przepisujemy ci¹g znaków z wejœcia do tablicy i sprawdzamy
    // czy jest to liczba reprezentuj¹ca energiê
    if (endOfLine(*c) &&
        validEnergy(tmpEnergy, tmpEnergyLength,
                    maxEnergyArray, maxEnergyLength)) {
        setEnergy(targetHistory, tmpEnergy, tmpEnergyLength);
    }
    else {
        scrollLine(c);
        error();
    }

    free(tmpEnergy);
}

// Procedura obs³uguj¹ca dzia³anie komendy ENERGY
//  hisTree         - korzeñ drzewa dopuszczalnych historii
//  maxEnergyArray  - tablica z cyframi maksymalnej energii
//  maxEnergyLength - rozmiar tej tablicy
void cmdEnergy(History hisTree, int* maxEnergyArray,
               int maxEnergyLength) {
    char c, *tmpHistory;
    int tmpHistoryLength;
    History historyLocation;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    if ((!endOfLine(c) && c != ' ') || noHistoryGiven(c, tmpHistoryLength) ||
        (tmpHistoryLength == 0 && c == ' ')) {
        while (!endOfLine(c) && c != EOF) c = getchar();
        error();
    }
    else {
        // Podano historiê
        historyLocation = hisTree;

        findHistory(&historyLocation, tmpHistory, tmpHistoryLength);

        // Sprawdzamy, czy podana historia jest dopuszczalna
        // i czy podano jeden czy dwa argumenty
        if (historyLocation && endOfLine(c)) {
            // Podano dok³adnie jeden argument
            checkEnergy(historyLocation);
        }
        else if (historyLocation && c == ' ') {
            // Potencjalnie podano dwa argumenty
            newEnergyProcedure(historyLocation, maxEnergyArray,
                               maxEnergyLength, &c);
        }
        else {
            // Podana historia nie jest dopuszczalna
            scrollLine(&c);
            error();
        }
    }

    free(tmpHistory);
}
