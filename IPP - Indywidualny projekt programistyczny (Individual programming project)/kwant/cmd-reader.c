/* Modu³ cmd-reader zawiera funkcje potrzebne
 * do czytania komend z wejœcia, rozpoznawania ich
 * oraz do wywo³ania odpowiadaj¹cych im funkcji z innych modu³ów,
 * które kontynuowaæ bêd¹ czytanie wejœcia i wykonywanie komend. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmd-reader.h"
#include "utils.h"
#include "declare.h"
#include "valid.h"
#include "remove.h"
#include "energy.h"
#include "equal.h"
#include "cmd-info.h"

// Funkcja jest stra¿nikiem pêtli czytaj¹cej ci¹g
// znaków bêd¹cy potencjaln¹ pêtl¹. Warunki zawarte
// w tej formule logicznej s¹ pierwsz¹ "lini¹ obrony"
// przed nieprawid³owymi danymi wejœciowymi
//  c - ostatnio przeczytany znak
//  i - obrót pêtli (jednoczeœnie liczba przeczytanych
//      dotychczas znaków)
static bool keepReading(char c, int i) {
 return (c != '#' &&
         c != ' ' &&
         c != EOF &&
         !endOfLine(c) &&
         i < MAX_CMD_LENGTH);
}

// Funkcja sprawdza, czy:
//      - wpisana komenda jest za d³uga lub za krótka
//      - pomimo poprawnej d³ugoœci zakoñczy³a siê
//        z powodu niedopuszczalnego znaku, a nie spacji
//  c - ostatnio przeczytany znak
//  i - obrót pêtli (jednoczeœnie liczba przeczytanych
//      dotychczas znaków)
static bool invalidLengthOrChar(char c, int i) {
    return ((i >= INITIAL_STRING_SIZE &&
            i <= MAX_CMD_LENGTH &&
            c != ' ') ||
            i < INITIAL_STRING_SIZE);
}

// Funkcja czyta znaki z wejœcia, które potencjalnie s¹ komend¹,
// komentarzem lub b³êdnym wyra¿eniem. Sprawdza równie¿,
// czy skoñczy³y siê dane. Zwraca kod zdefiniowany w pliku cmd-info.h
// lub d³ugoœæ potencjalnej komendy, jako informacja dla funkcji
// recognizeCommand()
//  command - wskaŸnik do tablicy przeznaczonej na komendê
static int readCommand(char** command) {
    char c;
    int i, size;

    size = INITIAL_STRING_SIZE;

    *command = (char*)malloc(size * sizeof(char));
    // Sprawdzamy, czy alokacja siê powiod³a.
    if (!(*command))
        exit(1);

    i = 0;
    c = getchar();

    // Pêtla zatrzyma siê zarówno gdy ci¹g znaków jest potencjalnie
    // poprawn¹ komend¹, jak i gdy na pewno ni¹ nie jest
    while (keepReading(c, i))
        addCharToString(command, &c, &i, &size);

    // Nadajemy odpowiedni kod zale¿nie od sytuacji
    if (i == 0 && c == EOF) {
        i = EOF;
    }
    else if (i == 0 && (c == '#' || c == '\n')) {
        i = IGNORE;
        scrollLine(&c);
    }
    else if (invalidLengthOrChar(c, i)) {
        i = ERROR;
        scrollLine(&c);
    }

    return i;
}

// Funkcja sprawdza czy dwa stringi w postaci tablic charów
// s¹ identyczne
static bool compareStrings(char* string1, int string1length,
                           char* string2, int string2length) {
    bool same;

    //Jesli sa roznych dlugosci, to wiadomo, ze sa rozne
    same = (string1length == string2length);

    for (int i = 0; i < string1length && same; i++) {
        same = string1[i] == string2[i];
    }

    return same;
}

// Zale¿nie od obrotu pêtli, w której siê znajduje, funkcja ta wywo³uje
// funkcjê compareStrings do porównania ci¹gu z wejœcia z odpowiedni¹ komend¹.
// Jeœli rzeczywiœcie jest to komenda, to funkcja ustawia wartoœæ zmiennej
// whichCmd na kod odpowiedniej komendy.
//  i           - obrót pêtli (jednoczeœnie kod sprawdzanej aktualnie komendy)
//  whichCmd    - wskaŸnik na zmienn¹, której wartoœæ bêdzie kodem komendy
//  command     - tablica z ci¹giem znaków z wejœcia
//  cmdLength   - wielkoœæ tej tablicy
static void checkCmd(int i, int* whichCmd, char* command, int cmdLength) {
    switch (i) {
        case CMD_ID_DECLARE:
            if (compareStrings(command, cmdLength, "DECLARE", DEC_LEN))
                *whichCmd = CMD_ID_DECLARE;
            break;
        case CMD_ID_REMOVE:
            if (compareStrings(command, cmdLength, "REMOVE", REM_LEN))
                *whichCmd = CMD_ID_REMOVE;
            break;
        case CMD_ID_VALID:
            if (compareStrings(command, cmdLength, "VALID", VAL_LEN))
                *whichCmd = CMD_ID_VALID;
            break;
        case CMD_ID_ENERGY:
            if (compareStrings(command, cmdLength, "ENERGY", ENE_LEN))
                *whichCmd = CMD_ID_ENERGY;
            break;
        case CMD_ID_EQUAL:
            if (compareStrings(command, cmdLength, "EQUAL", EQU_LEN))
                *whichCmd = CMD_ID_EQUAL;
            break;
        default:
            // Ci¹g z wejœcia mia³ odpowiedni¹ d³ugoœæ i nie zawiera³
            // w oczywisty sposób niedopuszczalnych znaków, ale,
            //  jak siê okaza³o nie by³ komend¹
            *whichCmd = ERROR;
            // Mozemy wywolac getchar(), poniewaz w tym momencie
            // ostatnim pobranym znakiem na pewno jest spacja
            char c = getchar();
            scrollLine(&c);
            break;
    }
}

// Funkcja wywo³uje funkcjê czytaj¹c¹ znaki z wejœcia, funkcjê sprawdzaj¹c¹
// czy te znaki s¹ komend¹, i zwraca odpowiedni kod zdefiniowany w cmd-info.h
int recognizeCommand() {
    char *command;
    int whichCmd, cmdLength;

    // cmdLength przyjmie wartoœæ kodu lub
    // rzeczywistej d³ugoœci ci¹gu znaków
    cmdLength = readCommand(&command);
    if (cmdLength == EOF)
        whichCmd = EOF;
    else if (cmdLength == ERROR)
        whichCmd = ERROR;
    else if (cmdLength == IGNORE)
        whichCmd = IGNORE;
    else // <=> cmdLength to nie kod, a rzeczywista d³ugoœæ wczytanego ci¹gu
        whichCmd = NONE;

    for (int i = CMD_ID_DECLARE; whichCmd == NONE; i++) {
        checkCmd(i, &whichCmd, command, cmdLength);
    }

    free(command);

    return whichCmd;
}

// Na podstawie kodu ustawionego w funkcji recognizeCommand,
// funkcja wywo³uje adekwatn¹ funkcjê odpowiadaj¹c¹ komendzie
//  cmd             - kod komendy
//  continueProgram - wskaŸnik na zmienn¹, która jest stra¿nikiem
//                    pêtli obs³uguj¹cej program
//  (potrzebne dla wywo³ywanych funkcji):
//  hisTree         - korzeñ drzewa
//                    z dopuszczalnymi historiami
//  maxEnergyArray  - tablica z cyframi liczby 2^64 - 1
//  maxEnergyLength - rozmiar tej tablicy
void runCommand(int cmd, History hisTree, int* maxEnergyArray,
                int maxEnergyLength, bool* continueProgram) {
    switch (cmd) {
        case CMD_ID_DECLARE:
            cmdDeclare(hisTree);
            break;
        case CMD_ID_REMOVE:
            cmdRemove(hisTree);
            break;
        case CMD_ID_VALID:
            cmdValid(hisTree);
            break;
        case CMD_ID_ENERGY:
            cmdEnergy(hisTree, maxEnergyArray, maxEnergyLength);
            break;
        case CMD_ID_EQUAL:
            cmdEqual(hisTree);
            break;
        case ERROR:
            error();
            break;
        case EOF:
            *continueProgram = false;
            break;
        default:
            break;
    }
}
