/* Modu³ valid zawiera funkcje potrzebne
 * do obs³ugi komendy VALID. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "valid.h"
#include "utils.h"
#include "tree-search.h"
#include "structure.h"

// Lokalna funkcja zwraca wartoœæ logiczn¹ odpowiadaj¹c¹ informacji o tym,
// czy historia podana na wejœcie jest aktualnie dopuszczalna.
//  hisTree             - korzeñ drzewa przechowuj¹cego dopuszczalne
//                        historie kwantowe
//  testedHistory       - tablicowa reprezentacja historii wczeœniej
//                        pobranej z wejœcia
//  testedHistoryLength - d³ugoœæ historii wczeœniej pobranej z wejœcia
//                        (rozmiar tablicy testedHistory)
static bool historyIsValid(History hisTree, char* testedHistory,
                           int testedHistoryLength) {
    History current;

    current = hisTree;

    findHistory(&current, testedHistory, testedHistoryLength);

    // Jeœli funkcja findHistory dotar³a do NULLA, to znaczy,
    // ¿e podanej na wejœcie historii nie ma w drzewie,
    // wiêc jest niedopuszczalna.
    return (current != NULL);
}

// Procedura do obs³ugi komendy VALID
//  hisTree - adres korzenia drzewa z dopuszczalnymi historiamioraz, jesli okaze sie, ze tak jest, wywoluje funkcje sprawdzajaca czy jest to dopuszczalna historia. hisTree - korzen drzewa przechowujacego dopuszczalne historie kwantowe
void cmdValid(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Sprawdzamy, czy ci¹g znaków pobranych z wejœcia
    // potencjalnie jest histori¹ kwantow¹.
    // Jeœli pêtla w funkcji prepareTemporaryHistoryArray zakoñczy³a siê
    // z powodu innego znaku ni¿ znak nowej linii, to znaczy, ¿e
    // ci¹g podany na wejœciu nie by³ potencjalnie dopuszczaln¹ histori¹.
    // Nieprawid³owym u¿yciem komendy VALID jest te¿ niepodanie argumentu.
    if (!endOfLine(c) || noHistoryGiven(c, tmpHistoryLength)){
        scrollLine(&c);
        error();
    }
    else {
        if (historyIsValid(hisTree, tmpHistory, tmpHistoryLength))
            printf("YES\n");
        else
            printf("NO\n");
    }

    free(tmpHistory);
}
