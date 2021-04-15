/* Modu� equal zawiera funkcje potrzebne
 * do obs�ugi komendy EQUAL. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "equal.h"
#include "utils.h"
#include "relation-utils.h"
#include "tree-search.h"
#include "structure.h"

// Funkcja zwraca �redni� arytmetyczn� z dw�ch danych
// warto�ci typu uint64_t.
// Pierwsze dwa sk�adniki to ilorazy tych warto�ci dzielonych
// przez 2 przy u�yciu operacji bitowej przesuwana o jeden bit w prawo.
// Dzielenie jest wykonywane przed dodaniem tych warto�ci w celu
// unikni�cia przekroczenia warto�ci 2^64 - 1 b�d�cej maksymaln� warto�ci� typu
// uint64_t.  Ostatni sk�adnik jest r�wny 1 je�li obie warto�ci s�
// nieparzyste i r�wny 0 je�li chocia� jedna jest parzysta.
// W j�zyku C warto�ci zaokr�glane s� w d�, wi�c gdyby obie
// warto�ci by�y nieparzyste, to
// (val1 / 2) + (val2 / 2) == (val1 + val2) / 2 - 1
// Sprawdzanie parzysto�ci wykonywane jest przy u�yciu operacji bitowej AND,
// kt�rej wynikiem jest warto�� liczby, kt�rej reprezentacja bitowa
// zawiera jedynki tylko w tych miejscach, w kt�rych oba sk�adniki zawiera�y
// jedynki. Wszystkie nieparzyste liczby maj� jedynk� na pierwszym miejscu
// w reprezentacji bitowej, wi�c warto�ci� dziesi�tn� wyniku operacji &
// jakiejkolwiek liczby ca�kowitej w reprezentacji bitowej z 1 w reprezentacji
// bitowej b�dzie 1 lub 0
static uint64_t arithmeticMean(uint64_t value1, uint64_t value2) {
    return (value1 >> 1) + (value2 >> 1) + ((value1 & 1) & (value2 & 1));
}

// Funkcja zr�wnuje energi� historii history1 z pust� energi�
// historii history2, czyli w rzeczywisto�ci przypisuj� energi�
// history1 do history2. Tym samym w��czamy history2 do relacji,
// do kt�rej nale�y history1
//  history1    - historia z przypisan� energi�
//  history2    - historia bez przypisanej energii
static void shareEnergy(History history1, History history2) {
    History nextCopy;

    history2->energy = history1->energy;
    nextCopy = history1->next;

    connectEnds(history1, history2);
    connectEnds(history2, nextCopy);
}

// Funkcja "scala" relacje, do kt�rych nale�a�y historie,
// doka�czaj�c tym samym proces zr�wnywania energii
//  history1    - historia, do kt�rej najpierw przypisali�my now� energi�
//  history2    - historia, kt�rej relacj� b�dziemy dopiero "wpina�" w relacj�,
//                do kt�rej nale�y history1
//  energy      - wska�nik do nowej energii, kt�r� chcemy przypisa� historiom
//                z relacji, do kt�rej nale�y history2
static void mergeRelations(History history1, History history2, uint64_t* energy) {
    History tmpHistory1, tmpHistory2, history2Start, history2End;

    // Znajdujemy pocz�tek i koniec listy dwukierunkowej,
    // b�d�cej reprezentacj� relacji, w kt�rej jest history2
    history2End = history2;
    tmpHistory2 = history2->next;

    while (tmpHistory2) {
        tmpHistory2->energy = energy;
        history2End = tmpHistory2;
        tmpHistory2 = tmpHistory2->next;
    }

    history2Start = history2;
    tmpHistory2 = history2->prev;

    while (tmpHistory2) {
        tmpHistory2->energy = energy;
        history2Start = tmpHistory2;
        tmpHistory2 = tmpHistory2->prev;
    }

    // Wpinamy ca�� relacj�, w kt�rej jest history2,
    // pomi�dzy w�ze� history1, a jego nast�pnik w
    // li�cie jego relacji
    tmpHistory1 = history1->next;

    connectEnds(history1, history2Start);
    connectEnds(history2End, tmpHistory1);
}

// Funkcja zr�wnuje energie historii history1 i history2, przy czym
// obie maj� ju� przypisan� energi�
static void balanceEnergy(History history1, History history2) {
    uint64_t *energy1, *energy2;

    energy1 = history1->energy;
    energy2 = history2->energy;

    // Liczymy �redni� z energii tych historii i przypisujemy
    // j� najpierw do history1, a pami�� po energii history2
    // zwalniamy, poniewa� chcemy przypisa� now� energi� wszystkim
    // historiom z relacji, w kt�rej znajduje si� history2
    *(energy1) = arithmeticMean(*(energy1), *(energy2));
    free(energy2);
    history2->energy = energy1;

    mergeRelations(history1, history2, energy1);
}

// Procedura wywo�uj�ca odpowiedni� funkcj� potrzebn�
// do zr�wnania energii dw�ch historii, w przypadku
// gdy przynajmniej jedna z nich ma ju� przypisan� energi�
static void equalizeEnergyProcedure(History history1, History history2) {
    if (!(history1->energy) && !(history2->energy)) {
        error();
    }
    else {
        if (!(history1->energy)) {
            shareEnergy(history2, history1);
        }
        else if (!(history2->energy)) {
            // W tym przypadku odwrotne wywo�anie
            // z powodu implementacji funkcji shareEnergy
            shareEnergy(history1, history2);
        }
        else if (history1->energy != history2->energy) {
            balanceEnergy(history1, history2);
        }

        printf("OK\n");
    }
}

// Procedura wykonuje czynno�ci niezb�dne w sytuacji, gdy u�ytkownik
// poda� na wej�cie dwie historie
//  hisTree     - korze� drzewa dopuszczalnych historii
//  his1        - tablica z reprezentacj� historii 1
//  his2        - tablica z reprezentacj� historii 2
//  his1length  - rozmiar his1
//  his2Length  - rozmiar his2
static void twoHistoriesGiven(History hisTree, char* his1, char* his2,
                              int his1Length, int his2Length){
    History history1Location, history2Location;

    // Sprawdzamy, czy obie historie s� dopuszczalne
    history1Location = hisTree;
    history2Location = hisTree;
    findHistory(&history1Location, his1, his1Length);
    findHistory(&history2Location, his2, his2Length);

    if (!history1Location || !history2Location) {
        error();
    }
    else if (history1Location != history2Location) {
        equalizeEnergyProcedure(history1Location, history2Location);
    }
    else {
        // Gdy podano t� sam� dopuszczaln� histori� dwukrotnie
        printf("OK\n");
    }
}

// Procedura wykonuje czynno�ci niezb�dne w sytuacji, gdy u�ytkownik
// poda� na wej�cie co najmniej jedn� histori�
//  hisTree     - korze� drzewa dopuszczalnych historii
//  tmpHistory1 - tablica z reprezentacj� historii 1
//  his1length  - rozmiar his1
//  c           - wska�nik na zmienn� z ostatnio pobranym znakiem
static void oneHistoryGiven(History hisTree, char* tmpHistory1,
                     int tmpHis1Length, char* c){
    char *tmpHistory2;
    int tmpHis2Length;

    prepareTemporaryHistoryArray(&tmpHistory2, &tmpHis2Length, c);

    // Sczytujemy drugi argument i sprawdzamy, czy jest histori�
    if (!endOfLine(*c) || noHistoryGiven(*c, tmpHis2Length)) {
        scrollLine(c);
        error();
    }
    else {
        twoHistoriesGiven(hisTree, tmpHistory1, tmpHistory2,
                          tmpHis1Length, tmpHis2Length);
    }

    free(tmpHistory2);
}

// Procedura do obs�ugi komendy VALID
//  hisTree - korze� drzewa dopuszczalnych historii
void cmdEqual(History hisTree){
    char c, *tmpHistory1;
    int tmpHis1Length;

    prepareTemporaryHistoryArray(&tmpHistory1, &tmpHis1Length, &c);

    // Sczytujemy pierwszy argument i sprawdzamy czy jest histori�
    if (c != ' ') {
        scrollLine(&c);
        error();
    }
    else {
        oneHistoryGiven(hisTree, tmpHistory1, tmpHis1Length, &c);
    }

    free(tmpHistory1);
}
