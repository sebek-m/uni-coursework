/* Modu³ utils zawiera funkcje u¿ytkowe
 * wymagane w wiêkszoœci pozosta³ych modu³ów. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

// Funkcja wypisuje wiersz z napisem "ERROR"
// na standardowe wyjœcie diagnostyczne.
void error() {
    fprintf(stderr, "ERROR\n");
}

// Funkcja zwraca informacjê o tym,
// czy wartoœæ zmiennej c to znak nowej linii.
bool endOfLine(char c) {
    return (c == '\n');
}

// Funkcja pobiera znaki ze standardowego wejœcia
// a¿ do napotkania znaku nowej linii lub koæca pliku.
//  c - wskaŸnik na adres zmiennej,
//      która w funkcji wywo³uj¹cej scrollLine
//      s³u¿y³a do przechowywania ostatnio pobranego znaku.
void scrollLine(char* c) {
    while (!endOfLine(*c) && *c != EOF)
        *c = getchar();
}

// Funkcja zwraca wartoœæ logiczn¹ formu³y, która informuje,
// czy u¿ytkownik poda³ jak¹kolwiek historiê do odpowiedniej
// komendy.
//  c       - ostatni znak wczytany z wejœcia
//            podczas czytania historii
//  strLen  - rozmiar tablicy z wczytan¹ histori¹
bool noHistoryGiven(char c, int strLen) {
    return (endOfLine(c) && strLen == 0);
}

// Funkcja sprawdza, czy w aktualnie wype³nianym stringu jest miejsce
// i w przeciwnym razie zwiêksza przypisan¹ mu pamiêæ. Nastêpnie wpisuje
// ostatnio pobrany z wejœcia znak do pierwszej wolnej komórki
// oraz pobiera kolejny.
//  array   - wskaŸnik do tablicy charów, która reprezentuje wype³niany string
//  c       - wskaŸnik na adres ostatnio wczytanego znaku
//  i       - wskaŸnik na adres aktualnego indeksu
//  size    - wskaŸnik na adres zmiennej informuj¹cej o rozmiarze aktualnie
//            przypisanej pamieci
void addCharToString(char** array, char* c, int* i, int* size) {
    // Alokujemy trochê wiêcej pamiêci ni¿ jest dok³adnie potrzebne,
    // ¿eby zmniejszyæ liczbê wywo³añ drogiej procedury alokacji pamiêci.
    if (*i == *size) {
        *size = 1 + (*size) * 3 / 2;

        *array = (char*)realloc(*array, (*size) * sizeof(char));
        // Sprawdzamy, czy pamiêæ zosta³a poprawnie zwiêkszona
        if (!(*array))
            exit(1);
    }

    (*array)[*i] = *c;
    (*i)++;
    *c = getchar();
}

// Funkcja sprawdza, czy dany znak reprezentuje poprawny stan kwantowy
// czyli czy nale¿y do zbioru {'0', '1', '2', '3'}. Operacje porównania
// w funkcji bierze pod uwagê kod ASCII danego znaku.
//  state   - sprawdzany znak
bool correctQuantumState(char state) {
    return (state >= '0' && state <= '3');
}

// Funkcja czyta z wejœcia znaki, które potencjalnie
// reprezentuj¹ poprawne stany kwantowe i wpisuje je
// kolejno do tablicy z dynamicznie alokowan¹ pamiecia,
// tym samym tworz¹c tymczasow¹ reprezezentacjê podanej
// na wejœcie historii kwantowej.
// tmpHistory       - wskaŸnik na tablicê, w które ma byæ
//                    przechowywana historia
// tmpHistoryLength - wskaŸnik na zmienn¹ przechowuj¹c¹ rozmiar
//                    tworzonej tablicy
// c                - wskaŸnjk na wczeœniej zadeklarowan¹ zmienn¹,
//                    która przechowywaæ bêdzie ostatnio wczytany znak
void prepareTemporaryHistoryArray(char** tmpHistory, int* tmpHistoryLength,
                                  char* c) {
    int size;

    *c = getchar();
    size = INITIAL_STRING_SIZE;
    // Alokujemy trochê pamiêci, by ograniczyæ liczbê póŸniejszych alokacji.
    *tmpHistory = (char*)malloc(size * sizeof(char));
    // Sprawdzamy powodzenie przypisania pamiêci
    if (!(*tmpHistory))
        exit(1);

    *tmpHistoryLength = 0;

    // Pêtla zatrzyma siê, gdy zostanie napotkany znak niebêd¹cy reprezentacj¹
    // stanu kwantowaego.
    while (correctQuantumState(*c))
        addCharToString(tmpHistory, c, tmpHistoryLength, &size);
}
