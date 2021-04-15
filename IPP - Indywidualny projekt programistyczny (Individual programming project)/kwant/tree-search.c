/* Modu� tree-search zawiera funkcje potrzebne
 * do iteracyjnego przemieszczania si� po drzewie
 * dopuszczalnych historii. */

#include <stdio.h>
#include "tree-search.h"
#include "structure.h"

// Funkcja wykonuje jeden krok iteracji, czyli ustawia
// wska�nik current na odpowiedniego syna.
//  current     - wska�nik, kt�rym poruszamy si� po drzewie
//  direction   - stan kwantowy informuj�cy na kt�rego syna
//                ma zosta� przestawiony wska�nik
void treeIterationStep(History* current, char direction) {
    switch (direction) {
        case '0':
            *current = (*current)->zero;
            break;
        case '1':
            *current = (*current)->one;
            break;
        case '2':
            *current = (*current)->two;
            break;
        case '3':
            *current = (*current)->three;
            break;
        default:
            break;
    }
}

// Funkcja szuka w drzewie historii zawartej w tablicy testedHistory
// i ustawia wska�nik hisTree na w�ze� odpowiadaj�cy za t� histori�.
// Je�li funkcja natrafi na NULL, to znaczy, �e historia nie jest dopuszczalna.
void findHistory(History* hisTree, char* testedHistory, int testedHistoryLength) {
    for (int i = 0; i < testedHistoryLength && *hisTree != NULL; i++)
        treeIterationStep(hisTree, testedHistory[i]);
}
