/* Modu� remove zawiera funkcje potrzebne
 * do obs�ugi komendy REMOVE. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "remove.h"
#include "utils.h"
#include "relation-utils.h"
#include "tree-search.h"
#include "structure.h"

// Funkcja "porz�dkuje" relacj�, do kt�rej nale�a�a usuwana historia,
// czyli "�ata dziur�" w li�cie, poprzez spi�cie poprzednika
// i nast�pnika usuwanego elementu.
//  hisTree - w�ze� z aktualnie usuwan� histori�
static void tidyRelation(History hisTree) {
    History prev, next;

    prev = hisTree->prev;
    next = hisTree->next;

    if (prev || next) {
        connectEnds(prev, next);
    }
    else if (hisTree->energy) { // && !prev && !next
        free(hisTree->energy);
    }
    // ^ Je�li energia tej historii nie jest zr�wnana
    // z �adn� inn�, to po prostu chcemy zwolni� po niej
    // pami��
}

// Funkcja rekurencyjnie usuwa drzewo. Tak naprawd� zawsze
// jest to poddrzewo, poniewa� korze� drzewa dopuszczalnych
// historii nie jest histori�.
static void deleteHisTree(History hisTree) {
    if (hisTree){
        deleteHisTree(hisTree->zero);
        deleteHisTree(hisTree->one);
        deleteHisTree(hisTree->two);
        deleteHisTree(hisTree->three);

        tidyRelation(hisTree);

        free(hisTree);
    }
}

// Funkcja ustawia odpowiedniego syna danego w�z�a na NULL
//  whichOne - znak reprezentuj�cy stan kwantowy
static void setSonToNull(History node, char whichOne) {
    switch (whichOne) {
        case '0':
            node->zero = NULL;
            break;
        case '1':
            node->one = NULL;
            break;
        case '2':
            node->two = NULL;
            break;
        case '3':
            node->three = NULL;
            break;
        default:
            break;
    }
}

// Procedura wykonuje czynno�ci potrzebne do usuni�cia wszystkich historii,
// kt�rych podana na wej�cie historia jest prefiksem.
//  hisTree             - korze� drzewa dopuszczalnych historii
//  testedHistory       - tablica z wcze�niej wczytan� z wej�cia histori�
//  testedHistoryLength - wielko�� tej tablicy
static void removeHistory(History hisTree, char* testedHistory,
                          int testedHistoryLength) {
    History current, father;

    current = hisTree;
    father = NULL;

    // Najpierw sprawdzamy, czy podana historia jest dopuszczalna.
    // Dodatkowo ustawiamy przy ka�dym obrocie wska�nik do ojca,
    // aby m�c potem ustawi� jego syna na NULL.
    for (int i = 0; i < testedHistoryLength && current != NULL; i++) {
        father = current;
        treeIterationStep(&current, testedHistory[i]);
    }

    // Je�li nie trafili�my na NULL to znaczy, �e historia jest dopuszczalna
    // i mo�na przyst�pi� do usuwania
    if (current != NULL){
        setSonToNull(father, current->value);
        deleteHisTree(current);
    }
}

// Procedura obs�uguj�ca dzia�anie komendy REMOVE
//  hisTree - korze� drzewa dopuszczalnych historii
void cmdRemove(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Czytamy ci�g znak�w z wej�cia i je�li jest on histori�,
    // czyli poprawnym argumentem, to przechodzimy do dalszej
    // fazy potencjalnego usuwania.
    if (!endOfLine(c) || noHistoryGiven(c, tmpHistoryLength)){
        scrollLine(&c);
        error();
    }
    else {
        removeHistory(hisTree, tmpHistory, tmpHistoryLength);
        printf("OK\n");
    }

    free(tmpHistory);
}

