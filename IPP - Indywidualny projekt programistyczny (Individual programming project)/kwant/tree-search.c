/* Modu³ tree-search zawiera funkcje potrzebne
 * do iteracyjnego przemieszczania siê po drzewie
 * dopuszczalnych historii. */

#include <stdio.h>
#include "tree-search.h"
#include "structure.h"

// Funkcja wykonuje jeden krok iteracji, czyli ustawia
// wskaŸnik current na odpowiedniego syna.
//  current     - wskaŸnik, którym poruszamy siê po drzewie
//  direction   - stan kwantowy informuj¹cy na którego syna
//                ma zostaæ przestawiony wskaŸnik
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
// i ustawia wskaŸnik hisTree na wêze³ odpowiadaj¹cy za tê historiê.
// Jeœli funkcja natrafi na NULL, to znaczy, ¿e historia nie jest dopuszczalna.
void findHistory(History* hisTree, char* testedHistory, int testedHistoryLength) {
    for (int i = 0; i < testedHistoryLength && *hisTree != NULL; i++)
        treeIterationStep(hisTree, testedHistory[i]);
}
