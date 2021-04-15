/* Modu� utils zawiera funkcje u�ytkowe
 * wymagane w wi�kszo�ci pozosta�ych modu��w. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

// Funkcja wypisuje wiersz z napisem "ERROR"
// na standardowe wyj�cie diagnostyczne.
void error() {
    fprintf(stderr, "ERROR\n");
}

// Funkcja zwraca informacj� o tym,
// czy warto�� zmiennej c to znak nowej linii.
bool endOfLine(char c) {
    return (c == '\n');
}

// Funkcja pobiera znaki ze standardowego wej�cia
// a� do napotkania znaku nowej linii lub ko�ca pliku.
//  c - wska�nik na adres zmiennej,
//      kt�ra w funkcji wywo�uj�cej scrollLine
//      s�u�y�a do przechowywania ostatnio pobranego znaku.
void scrollLine(char* c) {
    while (!endOfLine(*c) && *c != EOF)
        *c = getchar();
}

// Funkcja zwraca warto�� logiczn� formu�y, kt�ra informuje,
// czy u�ytkownik poda� jak�kolwiek histori� do odpowiedniej
// komendy.
//  c       - ostatni znak wczytany z wej�cia
//            podczas czytania historii
//  strLen  - rozmiar tablicy z wczytan� histori�
bool noHistoryGiven(char c, int strLen) {
    return (endOfLine(c) && strLen == 0);
}

// Funkcja sprawdza, czy w aktualnie wype�nianym stringu jest miejsce
// i w przeciwnym razie zwi�ksza przypisan� mu pami��. Nast�pnie wpisuje
// ostatnio pobrany z wej�cia znak do pierwszej wolnej kom�rki
// oraz pobiera kolejny.
//  array   - wska�nik do tablicy char�w, kt�ra reprezentuje wype�niany string
//  c       - wska�nik na adres ostatnio wczytanego znaku
//  i       - wska�nik na adres aktualnego indeksu
//  size    - wska�nik na adres zmiennej informuj�cej o rozmiarze aktualnie
//            przypisanej pamieci
void addCharToString(char** array, char* c, int* i, int* size) {
    // Alokujemy troch� wi�cej pami�ci ni� jest dok�adnie potrzebne,
    // �eby zmniejszy� liczb� wywo�a� drogiej procedury alokacji pami�ci.
    if (*i == *size) {
        *size = 1 + (*size) * 3 / 2;

        *array = (char*)realloc(*array, (*size) * sizeof(char));
        // Sprawdzamy, czy pami�� zosta�a poprawnie zwi�kszona
        if (!(*array))
            exit(1);
    }

    (*array)[*i] = *c;
    (*i)++;
    *c = getchar();
}

// Funkcja sprawdza, czy dany znak reprezentuje poprawny stan kwantowy
// czyli czy nale�y do zbioru {'0', '1', '2', '3'}. Operacje por�wnania
// w funkcji bierze pod uwag� kod ASCII danego znaku.
//  state   - sprawdzany znak
bool correctQuantumState(char state) {
    return (state >= '0' && state <= '3');
}

// Funkcja czyta z wej�cia znaki, kt�re potencjalnie
// reprezentuj� poprawne stany kwantowe i wpisuje je
// kolejno do tablicy z dynamicznie alokowan� pamiecia,
// tym samym tworz�c tymczasow� reprezezentacj� podanej
// na wej�cie historii kwantowej.
// tmpHistory       - wska�nik na tablic�, w kt�re ma by�
//                    przechowywana historia
// tmpHistoryLength - wska�nik na zmienn� przechowuj�c� rozmiar
//                    tworzonej tablicy
// c                - wska�njk na wcze�niej zadeklarowan� zmienn�,
//                    kt�ra przechowywa� b�dzie ostatnio wczytany znak
void prepareTemporaryHistoryArray(char** tmpHistory, int* tmpHistoryLength,
                                  char* c) {
    int size;

    *c = getchar();
    size = INITIAL_STRING_SIZE;
    // Alokujemy troch� pami�ci, by ograniczy� liczb� p�niejszych alokacji.
    *tmpHistory = (char*)malloc(size * sizeof(char));
    // Sprawdzamy powodzenie przypisania pami�ci
    if (!(*tmpHistory))
        exit(1);

    *tmpHistoryLength = 0;

    // P�tla zatrzyma si�, gdy zostanie napotkany znak nieb�d�cy reprezentacj�
    // stanu kwantowaego.
    while (correctQuantumState(*c))
        addCharToString(tmpHistory, c, tmpHistoryLength, &size);
}
