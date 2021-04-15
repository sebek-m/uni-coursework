/* Modu� energy zawiera funkcje potrzebne
 * do obs�ugi komendy ENERGY. */

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
// input jest cyfr� (tzn. czy jego kod ASCII
// jest z zakresu 0-9).
static bool inputIsDigit(char input) {
    return (input >= '0' && input <= '9');
}

// Funkcja sprawdza, czy w aktualnie wype�nianej tablicy jest miejsce
// i w przeciwnym razie zwi�ksza przypisan� jej pami��. Nast�pnie wpisuje
// ostatnio pobran� z wej�cia cyfr� do pierwszej wolnej kom�rki
// oraz pobiera kolejn� cyfr�.
//  array   - wska�nik do wype�nianej tablicy integer'�w
//  c       - wska�nik na adres ostatnio wczytanego znaku
//  i       - wska�nik na adres aktualnego indeksu
//  size    - wska�nik na adres zmiennej informuj�cej o rozmiarze aktualnie
//            przypisanej pamieci
static void addDigitToArray(int** array, char* c, int* i, int* size) {
    // Alokujemy troch� wi�cej pami�ci ni� jest dok�adnie potrzebne,
    // �eby zmniejszy� liczb� wywo�a� drogiej procedury alokacji pami�ci.
    if (*i == *size) {
        *size = 1 + (*size) * 3 / 2;

        *array = (int*)realloc(*array, (*size) * sizeof(int));
        // Sprawdzamy, czy pami�� zosta�a poprawnie zwi�kszona
        if (!(*array))
            exit(1);
    }

    // Kod ASCII to integer wi�c, po odj�ciu od chara c, kt�ry
    // reprezentuje cyfr�, kodu chara '0', r�wnego 48,
    // dostaniemy kod ASCII r�wny cyfrze ze zmiennej c
    (*array)[*i] = (*c) - '0';
    (*i)++;
    *c = getchar();
}

// Funkcja czyta z wej�cia znaki, kt�re potencjalnie
// reprezentuj� cyfry i wpisuje je kolejno do tablicy
// z dynamicznie alokowan� pamiecia,
// tym samym tworz�c tymczasow� reprezezentacj� podanej
// na wej�cie energii.
// tmpEnergy        - wska�nik na tablic�, w kt�re ma by�
//                    przechowywana energia
// tmpEnergyLength  - wska�nik na zmienn� przechowuj�c� rozmiar
//                    tworzonej tablicy
// c                - wska�njk na wcze�niej zadeklarowan� zmienn�,
//                    kt�ra przechowywa� b�dzie ostatnio wczytany znak
static void prepareTemporaryEnergyArray(int** tmpEnergy, int* tmpEnergyLength, char* c) {
    int size;

    *c = getchar();
    size = INITIAL_ARRAY_SIZE;

    *tmpEnergy = (int*)malloc(size * sizeof(int));
    // Sprawdzamy czy alokacja pami�ci si� powiod�a
    if (!(*tmpEnergy))
        exit(1);

    *tmpEnergyLength = 0;

    // P�tla zatrzyma si�, gdy zostanie napotkany znak nieb�d�cy cyfr�.
    while (inputIsDigit(*c))
        addDigitToArray(tmpEnergy, c, tmpEnergyLength, &size);
}

// Funkcja sprawdza, czy energia, wcze�niej podana na wej�cie,
// mie�ci si� w zakresie liczb ca�kowitych [1, 2^64 - 1]
//  tmpEnergy       - tablica z cyframi energii z wej�cia
//  tmpEnergyLength - rozmiar tej tablicy
//  maxEnergyArray  - tablica z cyframi liczby 2^64 - 1
//  maxEnergyLength - rozmiar tej tablicy
static bool validEnergy(int* tmpEnergy, int tmpEnergyLength, int* maxEnergyArray, int maxEnergyLength) {
    bool valid;

    // Sprawdzamy czy podano jak�kolwiek energi�, czy jej
    // reprezentacja ma niewi�cej cyfr ni� maksymalna energia
    // oraz czy nie wpisano wiod�cych zer
    valid = (tmpEnergyLength > 0 &&
             tmpEnergyLength <= maxEnergyLength &&
             tmpEnergy[0] != 0);

    // Podan� energi� chcemy por�wna� z maksymaln� energi� cyfra
    // po cyfrze tylko wtedy, gdy maj� tyle samo cyfr,
    // poniewa� jest to jedyna sytuacja,
    // kiedy r�nica mi�dzy ich wielko�ciami jest niepewna
    if (tmpEnergyLength == maxEnergyLength)
        for (int i = 0; i < maxEnergyLength && valid; i++)
            valid = (tmpEnergy[i] <= maxEnergyArray[i]);

    return valid;
}

// Funkcja zwraca liczb� typu uint64_t, kt�rej cyfry
// przechowywane s� w tablicy
//  array       - tablica z tymi cyframi
//  arrayLength - rozmiar tej tablicy
static uint64_t convertArrayToUint(int* array, int arrayLength) {
    uint64_t value, multiplier;

    value = 0;
    multiplier = 1;

    // Integery z tablicy s� tu niejawnie konwertowane na
    // typ uint64_t
    for (int i = arrayLength - 1; i >= 0; multiplier *= 10, i--) {
        value += array[i] * multiplier;
    }

    return value;
}

// Funkcja wypisuje na standardowe wyj�cie
// energi� danej historii, je�li posiada ona
// przypisan� energi�. W przeciwnym przypadku
// wypisuje ERROR na stderr
//  hisTree - adres do w�z�a odpowiadaj�cemu historii,
//            kt�rej energi� chcemy sprawdzi�
static void checkEnergy(History hisTree) {
    if (hisTree->energy != NULL) {
        printf("%"PRIu64"\n", *(hisTree->energy));
    }
    else {
        error();
    }
}

// Je�li dana historia ma ju� przypisan� energi�, to funkcja zmienia warto��
// tej energii, a w przeciwnym wypadku przypisuje jej now� energi�,
// wcze�niej alokuj�c pami�� na t� energi�
//  targetHistory   - historia, kt�rej chcemy przypisa� energi�
//  energy          - tablica z cyframi liczby odpowiadaj�cej energii,
//                    kt�r� chcemy przypisa�
//  energyLength    - rozmiar tej tablicy
static void setEnergy(History targetHistory, int* energy, int energyLength) {
    if (targetHistory->energy != NULL) {
        *(targetHistory->energy) = convertArrayToUint(energy, energyLength);
    }
    else {
        targetHistory->energy = (uint64_t*)malloc(sizeof(uint64_t));
        // Sprawdzamy czy alokacja si� powiod�a
        if (!(targetHistory->energy))
            exit(1);

        *(targetHistory->energy) = convertArrayToUint(energy, energyLength);
    }

    printf("OK\n");
}

// Procedura wykonywana w przypadku, gdy do komendy ENERGY podano dwa argumenty
//  targetHistory   - tablica z reprezentacj� wcze�niej podanej historii
//  maxEnergyArray  - tablica z cyframi maksymalnej energii
//  maxEnergyLength - rozmiar tej tablicy
//  c               - wska�nik na zmienn� ze znakiem ostatnio
//                    przeczytanym z wej�cia
static void newEnergyProcedure(History targetHistory, int* maxEnergyArray,
                               int maxEnergyLength, char* c) {
    int *tmpEnergy, tmpEnergyLength;

    prepareTemporaryEnergyArray(&tmpEnergy, &tmpEnergyLength, c);

    // Przepisujemy ci�g znak�w z wej�cia do tablicy i sprawdzamy
    // czy jest to liczba reprezentuj�ca energi�
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

// Procedura obs�uguj�ca dzia�anie komendy ENERGY
//  hisTree         - korze� drzewa dopuszczalnych historii
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
        // Podano histori�
        historyLocation = hisTree;

        findHistory(&historyLocation, tmpHistory, tmpHistoryLength);

        // Sprawdzamy, czy podana historia jest dopuszczalna
        // i czy podano jeden czy dwa argumenty
        if (historyLocation && endOfLine(c)) {
            // Podano dok�adnie jeden argument
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
