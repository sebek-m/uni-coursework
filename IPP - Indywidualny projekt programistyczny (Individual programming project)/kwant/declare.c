/* Modu� declare zawiera funkcje potrzebne
 * do obs�ugi komendy DECLARE. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "declare.h"
#include "utils.h"
#include "structure.h"

// Lokalna funkcja dodaj�ca w�ze� do drzewa z dopuszczalnymi
// historiami, kt�ry reprezentowa� b�dzie kolejny stan kwantowy
// w historii wcze�niej przeczytanej z wej�cia (i tym samym
// histori�, kt�rej odpowiada �cie�ka od korzenia do tego
// w�z�a w��cznie).
//  current - wska�nik na adres w�z�a, do kt�rego ma zosta�
//            podwieszony nowy w�ze�
//  state   - znak reprezentuj�cy stan kwantowy
static void newHistoryEntry(History* current, char state) {
    switch (state) {
        case '0':
            // Mo�e si� zdarzy� tak, �e prefiks podanej na wej�cie
            // historii ju� znajduje si� w drzewie
            if ((*current)->zero == NULL)
                (*current)->zero = newNode(state);
            // Po dodaniu w�z�a przesuwamy wska�nik na niego,
            // aby tworzy� histori� dalej.
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

// Procedura obs�uguj�ca komend� DECLARE.
//  hisTree - korze� drzewa z dopuszczalnymi historiami
void cmdDeclare(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Czytamy ci�g znak�w z wej�cia i sprawdzamy, czy jest to historia
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
