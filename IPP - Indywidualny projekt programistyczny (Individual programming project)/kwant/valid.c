/* Modu� valid zawiera funkcje potrzebne
 * do obs�ugi komendy VALID. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "valid.h"
#include "utils.h"
#include "tree-search.h"
#include "structure.h"

// Lokalna funkcja zwraca warto�� logiczn� odpowiadaj�c� informacji o tym,
// czy historia podana na wej�cie jest aktualnie dopuszczalna.
//  hisTree             - korze� drzewa przechowuj�cego dopuszczalne
//                        historie kwantowe
//  testedHistory       - tablicowa reprezentacja historii wcze�niej
//                        pobranej z wej�cia
//  testedHistoryLength - d�ugo�� historii wcze�niej pobranej z wej�cia
//                        (rozmiar tablicy testedHistory)
static bool historyIsValid(History hisTree, char* testedHistory,
                           int testedHistoryLength) {
    History current;

    current = hisTree;

    findHistory(&current, testedHistory, testedHistoryLength);

    // Je�li funkcja findHistory dotar�a do NULLA, to znaczy,
    // �e podanej na wej�cie historii nie ma w drzewie,
    // wi�c jest niedopuszczalna.
    return (current != NULL);
}

// Procedura do obs�ugi komendy VALID
//  hisTree - adres korzenia drzewa z dopuszczalnymi historiamioraz, jesli okaze sie, ze tak jest, wywoluje funkcje sprawdzajaca czy jest to dopuszczalna historia. hisTree - korzen drzewa przechowujacego dopuszczalne historie kwantowe
void cmdValid(History hisTree) {
    char c, *tmpHistory;
    int tmpHistoryLength;

    prepareTemporaryHistoryArray(&tmpHistory, &tmpHistoryLength, &c);

    // Sprawdzamy, czy ci�g znak�w pobranych z wej�cia
    // potencjalnie jest histori� kwantow�.
    // Je�li p�tla w funkcji prepareTemporaryHistoryArray zako�czy�a si�
    // z powodu innego znaku ni� znak nowej linii, to znaczy, �e
    // ci�g podany na wej�ciu nie by� potencjalnie dopuszczaln� histori�.
    // Nieprawid�owym u�yciem komendy VALID jest te� niepodanie argumentu.
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
