/* Modu³ declare zawiera funkcje potrzebne
 * do obs³ugi komendy DECLARE. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "declare.h"
#include "utils.h"
#include "structure.h"

// Lokalna funkcja dodaj¹ca wêze³ do drzewa z dopuszczalnymi
// historiami, który reprezentowaæ bêdzie kolejny stan kwantowy
// w historii wczeœniej przeczytanej z wejœcia (i tym samym
// historiê, której odpowiada œcie¿ka od korzenia do tego
// wêz³a w³¹cznie).
//  current - wskaŸnik na adres wêz³a, do którego ma zostaæ
//            podwieszony nowy wêze³
//  state   - znak reprezentuj¹cy stan kwantowy
static void newHistoryEntry(History* current, char state) {
    switch (state) {
        case '0':
            // Mo¿e siê zdarzyæ tak, ¿e prefiks podanej na wejœcie
            // historii ju¿ znajduje siê w drzewie
            if ((*current)->zero == NULL)
                (*current)->zero = newNode(state);
            // Po dodaniu wêz³a przesuwamy wskaŸnik na niego,
            // aby tworzyæ historiê dalej.
            *current = (*current)->zero;
            break;
        case '1':
            if ((*current)->one == NULL)
                (*current)->one = newNode(state);
            *current = (*current)->one;
            break;
        case '2':
            if ((*current)->two == NULL)
                (*current)->two = newNode(state);
            *current = (*current)->two;
            break;
        case '3':
            if ((*current)->three == NULL)
                (*current)->three = newNode(state);
            *current = (*current)->three;
            break;
        default:
            break;
    }
}

// Procedura obs³uguj¹ca komendê DECLARE.
//  hisTree - korzeñ drzewa z dopuszczalnymi historiami
void cmdDeclare(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Czytamy ci¹g znaków z wejœcia i sprawdzamy, czy jest to historia
    if (!endOfLine(c) || noHistoryGiven(c, tmpHistoryLength)) {
        scrollLine(&c);
        error();
    }
    else {
        History current = hisTree;

        for (int j = 0; j < tmpHistoryLength; j++)
            newHistoryEntry(&current, tmpHistory[j]);

        printf("OK\n");
    }

    free(tmpHistory);
}
