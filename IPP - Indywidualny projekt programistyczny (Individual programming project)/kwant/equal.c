/* Modu³ equal zawiera funkcje potrzebne
 * do obs³ugi komendy EQUAL. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "equal.h"
#include "utils.h"
#include "relation-utils.h"
#include "tree-search.h"
#include "structure.h"

// Funkcja zwraca œredni¹ arytmetyczn¹ z dwóch danych
// wartoœci typu uint64_t.
// Pierwsze dwa sk³adniki to ilorazy tych wartoœci dzielonych
// przez 2 przy u¿yciu operacji bitowej przesuwana o jeden bit w prawo.
// Dzielenie jest wykonywane przed dodaniem tych wartoœci w celu
// unikniêcia przekroczenia wartoœci 2^64 - 1 bêd¹cej maksymaln¹ wartoœci¹ typu
// uint64_t.  Ostatni sk³adnik jest równy 1 jeœli obie wartoœci s¹
// nieparzyste i równy 0 jeœli chocia¿ jedna jest parzysta.
// W jêzyku C wartoœci zaokr¹glane s¹ w dó³, wiêc gdyby obie
// wartoœci by³y nieparzyste, to
// (val1 / 2) + (val2 / 2) == (val1 + val2) / 2 - 1
// Sprawdzanie parzystoœci wykonywane jest przy u¿yciu operacji bitowej AND,
// której wynikiem jest wartoœæ liczby, której reprezentacja bitowa
// zawiera jedynki tylko w tych miejscach, w których oba sk³adniki zawiera³y
// jedynki. Wszystkie nieparzyste liczby maj¹ jedynkê na pierwszym miejscu
// w reprezentacji bitowej, wiêc wartoœci¹ dziesiêtn¹ wyniku operacji &
// jakiejkolwiek liczby ca³kowitej w reprezentacji bitowej z 1 w reprezentacji
// bitowej bêdzie 1 lub 0
static uint64_t arithmeticMean(uint64_t value1, uint64_t value2) {
    return (value1 >> 1) + (value2 >> 1) + ((value1 & 1) & (value2 & 1));
}

// Funkcja zrównuje energiê historii history1 z pust¹ energi¹
// historii history2, czyli w rzeczywistoœci przypisujê energiê
// history1 do history2. Tym samym w³¹czamy history2 do relacji,
// do której nale¿y history1
//  history1    - historia z przypisan¹ energi¹
//  history2    - historia bez przypisanej energii
static void shareEnergy(History history1, History history2) {
    History nextCopy;

    history2->energy = history1->energy;
    nextCopy = history1->next;

    connectEnds(history1, history2);
    connectEnds(history2, nextCopy);
}

// Funkcja "scala" relacje, do których nale¿a³y historie,
// dokañczaj¹c tym samym proces zrównywania energii
//  history1    - historia, do której najpierw przypisaliœmy now¹ energiê
//  history2    - historia, której relacjê bêdziemy dopiero "wpinaæ" w relacjê,
//                do której nale¿y history1
//  energy      - wskaŸnik do nowej energii, któr¹ chcemy przypisaæ historiom
//                z relacji, do której nale¿y history2
static void mergeRelations(History history1, History history2, uint64_t* energy) {
    History tmpHistory1, tmpHistory2, history2Start, history2End;

    // Znajdujemy pocz¹tek i koniec listy dwukierunkowej,
    // bêd¹cej reprezentacj¹ relacji, w której jest history2
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

    // Wpinamy ca³¹ relacjê, w której jest history2,
    // pomiêdzy wêze³ history1, a jego nastêpnik w
    // liœcie jego relacji
    tmpHistory1 = history1->next;

    connectEnds(history1, history2Start);
    connectEnds(history2End, tmpHistory1);
}

// Funkcja zrównuje energie historii history1 i history2, przy czym
// obie maj¹ ju¿ przypisan¹ energiê
static void balanceEnergy(History history1, History history2) {
    uint64_t *energy1, *energy2;

    energy1 = history1->energy;
    energy2 = history2->energy;

    // Liczymy œredni¹ z energii tych historii i przypisujemy
    // j¹ najpierw do history1, a pamiêæ po energii history2
    // zwalniamy, poniewa¿ chcemy przypisaæ now¹ energiê wszystkim
    // historiom z relacji, w której znajduje siê history2
    *(energy1) = arithmeticMean(*(energy1), *(energy2));
    free(energy2);
    history2->energy = energy1;

    mergeRelations(history1, history2, energy1);
}

// Procedura wywo³uj¹ca odpowiedni¹ funkcjê potrzebn¹
// do zrównania energii dwóch historii, w przypadku
// gdy przynajmniej jedna z nich ma ju¿ przypisan¹ energiê
static void equalizeEnergyProcedure(History history1, History history2) {
    if (!(history1->energy) && !(history2->energy)) {
        error();
    }
    else {
        if (!(history1->energy)) {
            shareEnergy(history2, history1);
        }
        else if (!(history2->energy)) {
            // W tym przypadku odwrotne wywo³anie
            // z powodu implementacji funkcji shareEnergy
            shareEnergy(history1, history2);
        }
        else if (history1->energy != history2->energy) {
            balanceEnergy(history1, history2);
        }

        printf("OK\n");
    }
}

// Procedura wykonuje czynnoœci niezbêdne w sytuacji, gdy u¿ytkownik
// poda³ na wejœcie dwie historie
//  hisTree     - korzeñ drzewa dopuszczalnych historii
//  his1        - tablica z reprezentacj¹ historii 1
//  his2        - tablica z reprezentacj¹ historii 2
//  his1length  - rozmiar his1
//  his2Length  - rozmiar his2
static void twoHistoriesGiven(History hisTree, char* his1, char* his2,
                              int his1Length, int his2Length){
    History history1Location, history2Location;

    // Sprawdzamy, czy obie historie s¹ dopuszczalne
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
        // Gdy podano tê sam¹ dopuszczaln¹ historiê dwukrotnie
        printf("OK\n");
    }
}

// Procedura wykonuje czynnoœci niezbêdne w sytuacji, gdy u¿ytkownik
// poda³ na wejœcie co najmniej jedn¹ historiê
//  hisTree     - korzeñ drzewa dopuszczalnych historii
//  tmpHistory1 - tablica z reprezentacj¹ historii 1
//  his1length  - rozmiar his1
//  c           - wskaŸnik na zmienn¹ z ostatnio pobranym znakiem
static void oneHistoryGiven(History hisTree, char* tmpHistory1,
                     int tmpHis1Length, char* c){
    char *tmpHistory2;
    int tmpHis2Length;

    prepareTemporaryHistoryArray(&tmpHistory2, &tmpHis2Length, c);

    // Sczytujemy drugi argument i sprawdzamy, czy jest histori¹
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

// Procedura do obs³ugi komendy VALID
//  hisTree - korzeñ drzewa dopuszczalnych historii
void cmdEqual(History hisTree){
    char c, *tmpHistory1;
    int tmpHis1Length;

    prepareTemporaryHistoryArray(&tmpHistory1, &tmpHis1Length, &c);

    // Sczytujemy pierwszy argument i sprawdzamy czy jest histori¹
    if (c != ' ') {
        scrollLine(&c);
        error();
    }
    else {
        oneHistoryGiven(hisTree, tmpHistory1, tmpHis1Length, &c);
    }

    free(tmpHistory1);
}
