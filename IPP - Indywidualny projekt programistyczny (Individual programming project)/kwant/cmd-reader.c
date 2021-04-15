/* Modu� cmd-reader zawiera funkcje potrzebne
 * do czytania komend z wej�cia, rozpoznawania ich
 * oraz do wywo�ania odpowiadaj�cych im funkcji z innych modu��w,
 * kt�re kontynuowa� b�d� czytanie wej�cia i wykonywanie komend. */

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

// Funkcja jest stra�nikiem p�tli czytaj�cej ci�g
// znak�w b�d�cy potencjaln� p�tl�. Warunki zawarte
// w tej formule logicznej s� pierwsz� "lini� obrony"
// przed nieprawid�owymi danymi wej�ciowymi
//  c - ostatnio przeczytany znak
//  i - obr�t p�tli (jednocze�nie liczba przeczytanych
//      dotychczas znak�w)
static bool keepReading(char c, int i) {
 return (c != '#' &&
         c != ' ' &&
         c != EOF &&
         !endOfLine(c) &&
         i < MAX_CMD_LENGTH);
}

// Funkcja sprawdza, czy:
//      - wpisana komenda jest za d�uga lub za kr�tka
//      - pomimo poprawnej d�ugo�ci zako�czy�a si�
//        z powodu niedopuszczalnego znaku, a nie spacji
//  c - ostatnio przeczytany znak
//  i - obr�t p�tli (jednocze�nie liczba przeczytanych
//      dotychczas znak�w)
static bool invalidLengthOrChar(char c, int i) {
    return ((i >= INITIAL_STRING_SIZE &&
            i <= MAX_CMD_LENGTH &&
            c != ' ') ||
            i < INITIAL_STRING_SIZE);
}

// Funkcja czyta znaki z wej�cia, kt�re potencjalnie s� komend�,
// komentarzem lub b��dnym wyra�eniem. Sprawdza r�wnie�,
// czy sko�czy�y si� dane. Zwraca kod zdefiniowany w pliku cmd-info.h
// lub d�ugo�� potencjalnej komendy, jako informacja dla funkcji
// recognizeCommand()
//  command - wska�nik do tablicy przeznaczonej na komend�
static int readCommand(char** command) {
    char c;
    int i, size;

    size = INITIAL_STRING_SIZE;

    *command = (char*)malloc(size * sizeof(char));
    // Sprawdzamy, czy alokacja si� powiod�a.
    if (!(*command))
        exit(1);

    i = 0;
    c = getchar();

    // P�tla zatrzyma si� zar�wno gdy ci�g znak�w jest potencjalnie
    // poprawn� komend�, jak i gdy na pewno ni� nie jest
    while (keepReading(c, i))
        addCharToString(command, &c, &i, &size);

    // Nadajemy odpowiedni kod zale�nie od sytuacji
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

// Funkcja sprawdza czy dwa stringi w postaci tablic char�w
// s� identyczne
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

// Zale�nie od obrotu p�tli, w kt�rej si� znajduje, funkcja ta wywo�uje
// funkcj� compareStrings do por�wnania ci�gu z wej�cia z odpowiedni� komend�.
// Je�li rzeczywi�cie jest to komenda, to funkcja ustawia warto�� zmiennej
// whichCmd na kod odpowiedniej komendy.
//  i           - obr�t p�tli (jednocze�nie kod sprawdzanej aktualnie komendy)
//  whichCmd    - wska�nik na zmienn�, kt�rej warto�� b�dzie kodem komendy
//  command     - tablica z ci�giem znak�w z wej�cia
//  cmdLength   - wielko�� tej tablicy
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
            // Ci�g z wej�cia mia� odpowiedni� d�ugo�� i nie zawiera�
            // w oczywisty spos�b niedopuszczalnych znak�w, ale,
            //  jak si� okaza�o nie by� komend�
            *whichCmd = ERROR;
            // Mozemy wywolac getchar(), poniewaz w tym momencie
            // ostatnim pobranym znakiem na pewno jest spacja
            char c = getchar();
            scrollLine(&c);
            break;
    }
}

// Funkcja wywo�uje funkcj� czytaj�c� znaki z wej�cia, funkcj� sprawdzaj�c�
// czy te znaki s� komend�, i zwraca odpowiedni kod zdefiniowany w cmd-info.h
int recognizeCommand() {
    char *command;
    int whichCmd, cmdLength;

    // cmdLength przyjmie warto�� kodu lub
    // rzeczywistej d�ugo�ci ci�gu znak�w
    cmdLength = readCommand(&command);
    if (cmdLength == EOF)
        whichCmd = EOF;
    else if (cmdLength == ERROR)
        whichCmd = ERROR;
    else if (cmdLength == IGNORE)
        whichCmd = IGNORE;
    else // <=> cmdLength to nie kod, a rzeczywista d�ugo�� wczytanego ci�gu
        whichCmd = NONE;

    for (int i = CMD_ID_DECLARE; whichCmd == NONE; i++) {
        checkCmd(i, &whichCmd, command, cmdLength);
    }

    free(command);

    return whichCmd;
}

// Na podstawie kodu ustawionego w funkcji recognizeCommand,
// funkcja wywo�uje adekwatn� funkcj� odpowiadaj�c� komendzie
//  cmd             - kod komendy
//  continueProgram - wska�nik na zmienn�, kt�ra jest stra�nikiem
//                    p�tli obs�uguj�cej program
//  (potrzebne dla wywo�ywanych funkcji):
//  hisTree         - korze� drzewa
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
