/* Modu³ remove zawiera funkcje potrzebne
 * do obs³ugi komendy REMOVE. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "remove.h"
#include "utils.h"
#include "relation-utils.h"
#include "tree-search.h"
#include "structure.h"

// Funkcja "porz¹dkuje" relacjê, do której nale¿a³a usuwana historia,
// czyli "³ata dziurê" w liœcie, poprzez spiêcie poprzednika
// i nastêpnika usuwanego elementu.
//  hisTree - wêze³ z aktualnie usuwan¹ histori¹
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
    // ^ Jeœli energia tej historii nie jest zrównana
    // z ¿adn¹ inn¹, to po prostu chcemy zwolniæ po niej
    // pamiêæ
}

// Funkcja rekurencyjnie usuwa drzewo. Tak naprawdê zawsze
// jest to poddrzewo, poniewa¿ korzeñ drzewa dopuszczalnych
// historii nie jest histori¹.
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

// Funkcja ustawia odpowiedniego syna danego wêz³a na NULL
//  whichOne - znak reprezentuj¹cy stan kwantowy
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

// Procedura wykonuje czynnoœci potrzebne do usuniêcia wszystkich historii,
// których podana na wejœcie historia jest prefiksem.
//  hisTree             - korzeñ drzewa dopuszczalnych historii
//  testedHistory       - tablica z wczeœniej wczytan¹ z wejœcia histori¹
//  testedHistoryLength - wielkoœæ tej tablicy
static void removeHistory(History hisTree, char* testedHistory,
                          int testedHistoryLength) {
    History current, father;

    current = hisTree;
    father = NULL;

    // Najpierw sprawdzamy, czy podana historia jest dopuszczalna.
    // Dodatkowo ustawiamy przy ka¿dym obrocie wskaŸnik do ojca,
    // aby móc potem ustawiæ jego syna na NULL.
    for (int i = 0; i < testedHistoryLength && current != NULL; i++) {
        father = current;
        treeIterationStep(&current, testedHistory[i]);
    }

    // Jeœli nie trafiliœmy na NULL to znaczy, ¿e historia jest dopuszczalna
    // i mo¿na przyst¹piæ do usuwania
    if (current != NULL){
        setSonToNull(father, current->value);
        deleteHisTree(current);
    }
}

// Procedura obs³uguj¹ca dzia³anie komendy REMOVE
//  hisTree - korzeñ drzewa dopuszczalnych historii
void cmdRemove(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Czytamy ci¹g znaków z wejœcia i jeœli jest on histori¹,
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

