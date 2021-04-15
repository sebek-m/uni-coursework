/** @file
 * Implementacje funkcji potrzebnych do czytania komend
 * ze standardowego wejścia, sprawdzania czy są poprawne
 * oraz do ich interpretacji.
 * Dodatkowo zawiera funkcję do wypisywania błędu,
 * w celu zamknięcia obsługi wejścia i wyjścia w jednym
 * module.
 *
 * @author Sebastian Miller
 * @date 16.05.2019
 */

#include "cmd-reader.h"
#include "utils.h"
#include "map.h"
#include "helper-strucs.h"
#include "translators.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/**
 * Początkowy rozmiar tablicy/napisu
 */
#define INITIAL_STRING_SIZE 6

/**
 * ID trybu czytania ID drogi.
 */
#define ROUTE_ID_MODE 0
/**
 * ID trybu czytania długości drogi.
 */
#define ROAD_LENGTH_MODE 1
/**
 * ID trybu czytania roku budowy drogi.
 */
#define BUILT_YEAR_MODE 2
/**
 * ID trybu czytania nazwy miasta.
 */
#define CITY_NAME_MODE 3

/**
 * ID pustej komendy, która jest czytana.
 */
#define BLANK_CMD 4
/**
 * ID rozpoznanej komendy tworzenia drogi krajowej o zadanym przebiegu.
 */
#define ROUTE_FROM_DESC_CMD 5
/**
 * ID rozpoznanej komendy tworzenia odcinka drogi.
 */
#define ADD_ROAD_CMD 6
/**
 * ID rozpoznanej komendy naprawiania odcinka drogi.
 */
#define REPAIR_ROAD_CMD 7
/**
 * ID rozpoznanej komendy proszenia o opis drogi.
 */
#define DESCRIPTION_CMD 8

/**
 * ID rozpoznanej komendy rozszerzania drogi krajowej.
 */
#define EXTEND_ROUTE_CMD 9

/**
 * ID rozpoznanej komendy usuwania odcinka drogi.
 */
#define REMOVE_ROAD_CMD 10

/**
 * ID rozpoznanej komendy tworzenia drogi krajowej.
 */
#define NEW_ROUTE_CMD 11

/**
 * ID rozpoznanej komendy usuwania drogi krajowej.
 */
#define REMOVE_ROUTE_CMD 12

/**
 * Równe ID ostatniej komendy, obecnie getRouteDescription.
 */
#define MAX_CMD_ID 12

/**
 * Długość napisu reprezentującego maksymalne ID drogi.
 */
#define MAX_ID_LENGTH 3
/**
 * Napis reprezentujący maksymalna długość drogi.
 */
#define MAX_ID_STRING "999"

/** @brief Sprawdza, czy c to koniec linii.
 * Sprawdza, czy co to znak końca linii.
 *
 * @param c        - sprawdzany znak;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool endOfLine(char c) {
    return c == '\n';
}

/** @brief Sprawdza czy znak to cyfra
 * Sprawdza czy znak to cyfra
 * @param c      - sprawdzany znak;
 * @return Wartość @p true jeśli tak, @p false jeśli nie
 */
static bool isDigit(char c) {
    return '0' <= c && c <= '9';
}

/** @brief Wywołuje odpowiednie sprawdzanie aktualnego znaku.
 * Sprawdza dany znak, w sposób zależny od aktualnego trybu czytania.
 *
 * @param c             - znak;
 * @param mode          - tryb czytania;
 * @param index         - aktualny indeks przy wpisywaniu do tablicy;
 * @return Wartość @p true jeśli znak poprawny, @p false jeśli nie.
 */
static bool charProperForMode(char c, int mode, size_t index) {
    bool ret;

    switch (mode) {
        case ROUTE_ID_MODE:
            ret = isDigit(c);
            break;
        case ROAD_LENGTH_MODE:
            ret = isDigit(c);
            break;
        case BUILT_YEAR_MODE:
            ret = isDigit(c) || (index == 0 && c == '-');
            break;
        case CITY_NAME_MODE:
            ret = validChar(c);
            break;
        default:
            ret = true;
            break;
    }

    return ret;
}

/** @brief Sprawdza, czy napis jest liczbą, która się mieści w danym typie.
 * Sprawdza, czy napis jest liczbą, która się mieści w danym typie.
 *
 * @param element           - element z napisem reprezentującym liczbę;
 * @param maxLength         - dlugość maksymalnej wartości typu;
 * @param maxDigits         - napis z reprezentacją maksymalnej wartości typu;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool validNumber(CmdElement* element, size_t maxLength,
                        const char* maxDigits) {
    bool valid;
    size_t elLength;
    char *elDigits;

    elLength = element->inputLength;
    elDigits = element->input;

    valid = elLength <= maxLength;

    // Nie chcemy porównywać liczb, gdy mają różną liczbę cyfr.
    if (elLength == maxLength)
        for (size_t i = 0; i < maxLength && valid; i++)
            valid = elDigits[i] <= maxDigits[i];

    return valid;
}

/** @brief Sprawdza czy napis reprezentuje dopuszczalny rok.
 * Sprawdza czy napis reprezentuje liczbę, która mieści się w zakresie int.
 *
 * @param element            - element z napisem reprezentującym liczbę;
 * @param maxInt             - wskaźnik na strukturę z maksymalnym intem;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool validYear(CmdElement* element, MaxNumber* maxInt) {
    bool valid, negative;
    size_t elLength, maxLength, i;
    char *elDigits, *maxDigits;

    elLength = element->inputLength;
    maxLength = maxInt->length;
    elDigits = element->input;
    maxDigits = maxInt->digits;
    negative = elDigits[0] == '-';

    valid = elLength <= maxLength || (elLength == maxLength + 1 && negative);

    if (elLength == maxLength ||
        (elLength == maxLength + 1 && negative)) {
        if (negative) {
            for (i = 0; i < maxLength - 1 && valid; i++)
                valid = elDigits[i + 1] <= maxDigits[i];

            if (valid)
                valid = elDigits[i + 1] <= maxDigits[i] + 1;
        }
        else {
            for (i = 0; i < maxLength && valid; i++)
                valid = elDigits[i] <= maxDigits[i];
        }
    }

    return valid;
}

/** @brief Sprawdza, czy napis nie ma zer wiodących.
 * Sprawdza, czy napis nie ma zer wiodących.
 *
 * @param element       - element z napisem;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool noLeadingZeros(CmdElement* element) {
    return (element->input)[0] != '0';
}

/** @brief Sprawdza, czy napis jest pusty.
 * Sprawdza czy element zawiera pusty napis.
 *
 * @param element      - wskaźnik na element;
 * @return Wartość @p true jeśli niepusty, @p false jeśli tak.
 */
static bool elementNotEmpty(CmdElement* element) {
    return element->input != NULL && element->inputLength > 0;
}

/** @brief Sprawdza czy napis jest poprawny.
 * Zależnie od trybu, ywołuje odpowiednią funkcję,
 * która sprawdza, czy napis jest poprawny.
 *
 * @param element      - element z napisem;
 * @param maxU         - struktura z maksymalnym unsigned int;
 * @param maxInt       - struktura z maksymalnym int;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool correctElement(CmdElement* element, MaxNumber* maxU,
                           MaxNumber* maxInt) {
    bool ret;

    ret = elementNotEmpty(element);

    if (ret) {
        switch (element->mode) {
            case ROUTE_ID_MODE:
                ret = noLeadingZeros(element) &&
                      validNumber(element, MAX_ID_LENGTH, MAX_ID_STRING);
                break;
            case ROAD_LENGTH_MODE:
                ret = noLeadingZeros(element) &&
                      validNumber(element, maxU->length, maxU->digits);
                break;
            case BUILT_YEAR_MODE:
                ret = noLeadingZeros(element) &&
                      validYear(element, maxInt);
                break;
            default:
                break;
        }
    }

    return ret;
}

/** @brief Tworzy nowy element.
 * Tworzy nowy element.
 *
 * @param input           - napis do elementu;
 * @param inputLength     - długość tego napisu;
 * @param mode            - tryb elementu;
 * @return Wskaźnik na stworzony element.
 */
static CmdElement* newElement(char* input, size_t inputLength, int mode) {
    CmdElement* new;

    new = (CmdElement*)malloc(sizeof(CmdElement));

    if (new) {
        new->input = input;
        new->inputLength = inputLength;
        new->mode = mode;
        new->nextElement = NULL;
    }

    return new;
}

/** @brief Dodaje '\0' na końcu napisu.
 * Dodaje '\0' na końcu napisu.
 *
 * @param str          - wskaźnik na wskaźnik na napis;
 * @param length       - długość napisu;
 * @param memSize      - mnoźnik pamięci zaalokowanej na napis;
 */
static void addNullCharacter(char** str, size_t length, size_t* memSize) {
        if (length == *memSize) {
            *memSize += 2;

            *str = (char*)realloc(*str, *memSize * sizeof(char));
        }

        (*str)[length] = '\0';
}

/** @brief Sprawdza czy znak jest średnikiem.
 * Sprawdza czy znak jest średnikiem.
 *
 * @param c        - znak;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool isDelimiter(char c) {
    return c == ';';
}

/** @brief Sprawdza, czy element ma być czytany dalej.
 * Na podstawie ostatnio przeczytanego znaku decyduje,
 * czy napis ma jeszcze potencjał na bycie poprawnym elementem
 * polecenia.
 *
 * @param c          - ostatnio wczytany znak;
 * @param mode       - tryb czytania;
 * @param i          - ostatni indeks;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool keepReading(char c, int mode, size_t i) {
    return !isDelimiter(c) &&
           !endOfLine(c) &&
           c != EOF &&
           charProperForMode(c, mode, i);

}

/** @brief Czyta znaki do końca linii.
 * Czyta znaki do końca linii.
 *
 * @param c        - wskaźnik na zmienną z czytanym znakiem;
 */
static void scrollLine(char* c) {
    while (!endOfLine(*c) && *c != EOF)
        *c = getchar();
}

/** @brief Sprawdza czy linia ma być zignorowana.
 * Sprawdza czy linia jest komentarzem lub pustą linią.
 *
 * @param c          - znak do decyzji;
 * @return Wartość @p true jeśli tak, @p false jeśli nie.
 */
static bool ignoreLine(char c) {
    return c == '#' || endOfLine(c);
}

/** @brief Dodaje znak do napisu.
 * Dodaje podany znak (*c) do tablicy, reprezentującej napis
 * oraz czyta kolejny znak do zmiennej c. W razie potrzeby,
 * zwiększa rozmiar pamięci zaalokowanej na napis.
 *
 * @param array             - wskaźnik na wskaźnik do napisu;
 * @param c                 - wskaźnik na znak;
 * @param i                 - wskaźnik na aktualny indeks w napisie
 *                            (czyli na jego długość);
 * @param size              - wskaźnik na zmienną informującą o rozmiarze
 *                            pamięci zaalokowanej na napis;
 * @return Wartość @p false jeśli alokacja pamięci przy powiększaniu tablicy
 * się nie powiodła, @p true w przeciwnym przypadku.
 */
static bool addCharToString(char** array, char* c, size_t* i, size_t* size) {
    if (*i == *size) {
        *size = 1 + (*size) * 3 / 2;

        *array = (char*)realloc(*array, (*size) * sizeof(char));
        if (*array == NULL)
            return false;
    }

    (*array)[*i] = *c;
    (*i)++;
    *c = getchar();

    return true;
}

/** @brief Sprawdza, czy właśnie przeczytany element miał poprawny format.
 * Sprawdza, czy właśnie przeczytany element miał poprawny format, czyli
 * czy czytanie go zakończyło się z powodu wystąpienia średnika lub znaku
 * nowej linii. Jeśli tak było, to dodaje zerowy znak na końcu napisu.
 * W przeciwnym przypadku zwalnia pamięć po napisie.
 *
 * @param c                 - ostatnio przeczytany znak;
 * @param index             - aktualny indeks w napisie (czyli jego długość);
 * @param input             - wskaźnik na wskaźnik na sprawdzany napis;
 * @param elementSize       - wskaźnik na mnoźnik pamięci zaalokowanej na napis;
 * @return
 */
static bool validFormat(char c, size_t index, char** input, size_t* elementSize) {
    if (isDelimiter(c) || endOfLine(c)) {
        addNullCharacter(input, index, elementSize);
        if (*input != NULL)
            return true;
    }

    if (*input != NULL)
        free(*input);
    return false;
}

/** @brief Sprawdza, czy ma być podjęta próba czytania elementu.
 * Sprawdza, czy linia wejścia ma być zignorowana, ponieważ jest
 * komentarze lub pustą linią, oraz czy dane wejściowe się skończyły.
 *
 * @param c                     - wskaźnik na ostatnio czytany znak;
 * @param mode                  - zmienna informująca o trybie czytania;
 * @param eofGuard              - wskaźnik na zmienną, która informuje o końcu pliku;
 * @param emptyLine             - wskaźnik na zmienną, która informuje o tym, czy linia jest pusta;
 * @return Wartość @p true, jeśli linia tekstu nie ma zostać przeczytana, @p false
 * jeśli ma zostać przeczytana.
 */
static bool doNotRead(char* c, int mode, bool* eofGuard, bool* emptyLine) {
    if (mode == BLANK_CMD) {
        if (*c == EOF) {
            *eofGuard = true;
            return true;
        }

        if (ignoreLine(*c)) {
            *emptyLine = true;
            scrollLine(c);
            return true;
        }
    }

    return false;
}

/** @brief Czyta jeden element komendy.
 * Czyta jeden element komendy.
 *
 * @param c          - wskaźnik na zmienną z czytanym znakiem;
 * @param mode       - tryb czytania;
 * @param eofGuard   - wskaźnik na zmienną z informacją o końcu pliku;
 * @param emptyLine  - wskaźnik na zmienną z informacją o pustej linii;
 * @return Wskaźnik na przeczytany element. NULL w przypadku błędu.
 */
static CmdElement* readOneElement(char* c, int mode, bool* eofGuard, bool* emptyLine) {
    size_t elementSize, i;
    char* input;
    CmdElement* new;

    *c = getchar();
    i = 0;
    elementSize = INITIAL_STRING_SIZE;

    // Czy linia jest komentarzem lub pustą linią
    if (doNotRead(c, mode, eofGuard, emptyLine))
        return NULL;

    input = (char*)malloc(elementSize * sizeof(char));
    if (input == NULL)
        return NULL;

    while (keepReading(*c, mode, i)) {
        if (!addCharToString(&input, c, &i, &elementSize))
            return NULL;
    }

    if (!validFormat(*c, i, &input, &elementSize))
        return NULL;

    new = newElement(input, i, mode);
    if (new == NULL)
        free(input);

    return new;
}

/** @brief Usuwa elementy komendy.
 * Zwalnia pamięć po wszystkich elementach komendy, poza pierwszym,
 * czyli samą nazwą komendy.
 *
 * @param cmd           - wskaźnik na komendę;
 */
static void deleteElements(CmdElement* cmd) {
    CmdElement *current, *next;

    current = cmd->nextElement;

    while (current != NULL) {
        free(current->input);
        next = current->nextElement;
        free(current);
        current = next;
    }
}

/** @brief Czyta jeden element i podłącza go do listy elementów komendy.
 * Czyta jeden element i podłącza go do listy elementów komendy.
 * Dodatkowo upewnia się, czy przeczytany element jest poprawny.
 *
 * @param element               - wskaźnik na zmienną wskaźnikową, reprezentującą
 *                                czytany element;
 * @param prev                  - wskaźnik na poprzedni element;
 * @param c                     - wskaźnik na ostatnio czytany znak;
 * @param mode                  - zmienna, informująca o trybie czytania;
 * @param maxU                  - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt                - wskaźnik na strukturę z maksymalnym intem;
 * @return Wartość @p true, jeśli przeczytany element jest poprawny,
 * @p false w przeciwnym przypadku lub jeśli wystąpił błąd alokacji pamięci.
 */
static bool readAndAppend(CmdElement** element, CmdElement* prev,
                          char* c, int mode, MaxNumber* maxU,
                          MaxNumber* maxInt) {
    *element = readOneElement(c, mode, NULL, NULL);
    prev->nextElement = *element;

    return *element != NULL && correctElement(*element, maxU, maxInt);
}

/** @brief Sprawdza, czy nazwa miasta się nie powtarza.
 * Sprawdza, czy element nie reprezentuje nazwy miasta, która już wystąpiła w komendzie.
 *
 * @param cmd                       - wskaźnik na komendę;
 * @param currentCity               - wskaźnik na element komendy, reprezentujący nazwę miasta;
 * @return Wartość @p true, jeśli miasto jest powtórzone, @p false w przeciwnym przypadku.
 */
static bool cityRepeated(CmdElement* cmd, CmdElement* currentCity) {
    CmdElement* checked;
    bool isDuplicate;

    checked = cmd;
    isDuplicate = false;

    while (checked != currentCity && !isDuplicate) {
        if (checked->mode == CITY_NAME_MODE)
            isDuplicate = strcmp(checked->input, currentCity->input) == 0;

        checked = checked->nextElement;
    }

    return isDuplicate;
}

/** @brief Czyta linię z opisem drogi krajowej do stworzenia.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do tworzenia drogi krajowej o zadanym przebiegu.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readRouteFromDesc(CmdElement *cmd, MaxNumber *maxU,
                              MaxNumber *maxInt, char *c) {
    CmdElement *cityName, *length, *year;
    bool lineEnd, success;

    lineEnd = false;
    success = readAndAppend(&cityName, cmd, c, CITY_NAME_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    while (success && !lineEnd) {
        success = readAndAppend(&length, cityName, c,
                                ROAD_LENGTH_MODE,
                                maxU, maxInt) && isDelimiter(*c);

        if (success) {
            success = readAndAppend(&year, length, c,
                                    BUILT_YEAR_MODE,
                                    maxU, maxInt) && isDelimiter(*c);

            if (success) {
                success = readAndAppend(&cityName, year, c, CITY_NAME_MODE,
                                        maxU, maxInt)
                          && (isDelimiter(*c) || endOfLine(*c))
                          && !cityRepeated(cmd, cityName);

                lineEnd = endOfLine(*c);
            }
        }
    }

    return success;
}

/** @brief Czyta linię z komendą do dodawania odcinka drogi.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do tworzenia odcinka drogi.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readAddRoad(CmdElement* cmd, MaxNumber* maxU, MaxNumber* maxInt, char* c) {
    CmdElement *city1, *city2, *length, *year;
    bool success;

    success = readAndAppend(&city1, cmd, c, CITY_NAME_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    if (success) {
        success = readAndAppend(&city2, city1, c, CITY_NAME_MODE,
                                maxU, maxInt) && isDelimiter(*c);

        if (success) {
            success = readAndAppend(&length, city2, c,
                                    ROAD_LENGTH_MODE,
                                    maxU, maxInt) && isDelimiter(*c);

            if (success) {
                success = readAndAppend(&year, length, c,
                                        BUILT_YEAR_MODE,
                                        maxU, maxInt) && endOfLine(*c);
            }
        }
    }

    return success;
}

/** @brief Czyta linię z komendą do reperowania odcinka drogi.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do reperowania odcinka drogi.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readRepairRoad(CmdElement* cmd, MaxNumber* maxU, MaxNumber* maxInt, char* c) {
    CmdElement *city1, *city2, *year;
    bool success;

    success = readAndAppend(&city1, cmd, c, CITY_NAME_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    if (success) {
        success = readAndAppend(&city2, city1, c, CITY_NAME_MODE,
                                maxU, maxInt) && isDelimiter(*c);

        if (success) {
            success = readAndAppend(&year, city2, c, BUILT_YEAR_MODE,
                                    maxU, maxInt) && endOfLine(*c);
        }
    }

    return success;
}

/** @brief Czyta numer drogi krajowej.
 * Czyta sam numer drogi krajowej i podłącza go do listy elementów komendy.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawny numer drogi,
 * @p false w przeciwnym przypadku.
 */
static bool readRouteId(CmdElement *cmd, MaxNumber *maxU, MaxNumber *maxInt, char *c) {
    CmdElement* routeId;
    bool success;

    success = readAndAppend(&routeId, cmd, c, ROUTE_ID_MODE,
                            maxU, maxInt) && endOfLine(*c);

    return success;
}

/** @brief Czyta linię z komendą do rozszerzania drogi krajowej.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do rozszerzania drogi krajowej.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readExtendRoute(CmdElement* cmd, MaxNumber* maxU, MaxNumber* maxInt, char* c) {
    CmdElement *routeId, *cityName;
    bool success;

    success = readAndAppend(&routeId, cmd, c, ROUTE_ID_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    if (success)
        success = readAndAppend(&cityName, routeId, c, CITY_NAME_MODE,
                                maxU, maxInt) && endOfLine(*c);

    return success;
}

/** @brief Czyta linię z komendą do usuwania odcinka drogi.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do usuwania odcinka drogi.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readRemoveRoad(CmdElement* cmd, MaxNumber* maxU,
                           MaxNumber* maxInt, char* c) {
    CmdElement *city1, *city2;
    bool success;

    success = readAndAppend(&city1, cmd, c, CITY_NAME_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    if (success)
        success = readAndAppend(&city2, city1, c, CITY_NAME_MODE,
                                maxU, maxInt) && endOfLine(*c);

    return success;
}

/** @brief Czyta linię z komendą do tworzenia nowej drogi krajowej.
 * Czyta i dzieli na elementy linię wejścia, która reprezentuje wywołanie
 * komendy do tworzenia nowej drogi krajowej.
 *
 * @param cmd               - wskaźnik na komendę;
 * @param maxU              - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt            - wskaźnik na strukturę z maksymalnym intem;
 * @param c                 - wskaźnik na ostatnio przeczytany znak;
 * @return Wartość @p true jeśli pomyślnie udało się przeczytać poprawnie
 * sformatowaną komendę, @p false w przeciwnym przypadku.
 */
static bool readNewRoute(CmdElement* cmd, MaxNumber* maxU,
                         MaxNumber* maxInt, char* c) {
    CmdElement *city1, *city2, *routeId;
    bool success;

    success = readAndAppend(&routeId, cmd, c, ROUTE_ID_MODE,
                            maxU, maxInt) && isDelimiter(*c);

    if (success) {
        success = readAndAppend(&city1, routeId, c, CITY_NAME_MODE,
                                maxU, maxInt) && isDelimiter(*c);

        if (success) {
            success = readAndAppend(&city2, city1, c, CITY_NAME_MODE,
                                    maxU, maxInt) && endOfLine(*c);
        }
    }

    return success;
}

/** @brief Wywołuje czytanie reszty polecenia.
 * Na postawie rozpoznanego polecenia wywołuje odpowiednią funkcję
 * czytającą to polecenie.
 *
 * @param cmd            - wskaźnik na strukturę z nazwą komendy;
 * @param maxU           - wskaźnik na strukturę z maksymalnym unsigned;
 * @param maxInt         - wskaźnik na strukturę z maksymalnym intem;
 * @param c              - wskaźnik na ostatnio czytany znak;
 * @return @p true jeśli się udało, @p false jeśli się nie udało lub niepoprawne polecenie.
 */
static bool readLine(CmdElement* cmd, MaxNumber* maxU, MaxNumber* maxInt, char* c) {
    bool success;

    success = false;

    switch (cmd->mode) {
        case ROUTE_FROM_DESC_CMD:
            success = readRouteFromDesc(cmd, maxU, maxInt, c);
            break;
        case ADD_ROAD_CMD:
            success = readAddRoad(cmd, maxU, maxInt, c);
            break;
        case REPAIR_ROAD_CMD:
            success = readRepairRoad(cmd, maxU, maxInt, c);
            break;
        case DESCRIPTION_CMD:
            success = readRouteId(cmd, maxU, maxInt, c);
            break;
        case EXTEND_ROUTE_CMD:
            success = readExtendRoute(cmd, maxU, maxInt, c);
            break;
        case REMOVE_ROAD_CMD:
            success = readRemoveRoad(cmd, maxU, maxInt, c);
            break;
        case NEW_ROUTE_CMD:
            success = readNewRoute(cmd, maxU, maxInt, c);
            break;
        case REMOVE_ROUTE_CMD:
            success = readRouteId(cmd, maxU, maxInt, c);
            break;
        default:
            break;
    }

    return success;
}

/** @brief Sprawdza, czy napis reprezentuje poprawne id drogi.
 * Sprawdza, czy napis reprezentuje poprawne id drogi krajowej.
 *
 * @param str               - wskaźnik na napis, reprezentujący id;
 * @param length            - długość tego napisu;
 * @return Wartość @p true, jeśli napis reprezentuje poprawne id,
 * @p false w przeciwnym przypadku.
 */
static bool checkIfRouteId(const char* str, size_t length) {
    size_t i;
    bool isId;

    isId = length < 4;

    if (isId) {
        if (!('1' <= str[0] && str[0] <= '9'))
            isId = false;

        for (i = 1; i < length && isId; i++) {
            isId = '0' <= str[i] && str[i] <= '9';
        }
    }

    return isId;
}

/** @brief Rozpoznaje komendę.
 * Rozpoznaje, którą z dostępnych komend jest linia,
 * która zaczyna się od niepustego napisu zawartego w cmd.
 *
 * @param cmd           - część linii sprzed pierwszego średnika
 * @return @p true jeśli rozpoznano, @p false jeśli nie.
 */
static bool recognizeCmd(CmdElement* cmd) {
    char* name;
    bool match;
    int cmdNum;

    name = cmd->input;
    match = false;

    for (cmdNum = BLANK_CMD + 1; cmdNum < MAX_CMD_ID + 1 && !match; cmdNum++) {
        switch (cmdNum) {
            case ROUTE_FROM_DESC_CMD:
                match = checkIfRouteId(name, cmd->inputLength);
                break;
            case ADD_ROAD_CMD:
                match = strcmp(name, "addRoad") == 0;
                break;
            case REPAIR_ROAD_CMD:
                match = strcmp(name, "repairRoad") == 0;
                break;
            case DESCRIPTION_CMD:
                match = strcmp(name, "getRouteDescription") == 0;
                break;
            case EXTEND_ROUTE_CMD:
                match = strcmp(name, "extendRoute") == 0;
                break;
            case REMOVE_ROAD_CMD:
                match = strcmp(name, "removeRoad") == 0;
                break;
            case NEW_ROUTE_CMD:
                match = strcmp(name, "newRoute") == 0;
                break;
            case REMOVE_ROUTE_CMD:
                match = strcmp(name, "removeRoute") == 0;
                break;
            default:
                break;
        }
    }

    if (match)
        cmd->mode = cmdNum - 1;

    return match;
}

/** @brief Uruchamia funkcję do tworzenia drogi na podstawie opisu.
 * Uruchamia funkcję do tworzenia drogi krajowej na podstawie opisu.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runRouteFromDescription(CmdElement* cmd, Map* map) {
    unsigned routeId;

    routeId = stringToUnsigned(cmd->input, cmd->inputLength, false);

    return newRouteFromDescription(map, routeId, cmd);
}

/** @brief Uruchamia funkcję do tworzenia odcinka drogi.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do tworzenia odcinka drogi.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runAddRoad(CmdElement* cmd, Map* map) {
    char *city1, *city2;
    unsigned length;
    int year;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    city1 = tmp->input;
    tmp = tmp->nextElement;

    city2 = tmp->input;
    tmp = tmp->nextElement;

    length = stringToUnsigned(tmp->input, tmp->inputLength, false);
    tmp = tmp->nextElement;

    year = stringToInt(tmp->input, tmp->inputLength);

    return addRoad(map, city1, city2, length, year);
}

/** @brief Uruchamia funkcję do naprawy odcinka drogi.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do naprawy odcinka drogi.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runRepairRoad(CmdElement* cmd, Map* map) {
    char *city1, *city2;
    int repairYear;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    city1 = tmp->input;
    tmp = tmp->nextElement;

    city2 = tmp->input;
    tmp = tmp->nextElement;

    repairYear = stringToInt(tmp->input, tmp->inputLength);

    return repairRoad(map, city1, city2, repairYear);
}

/** @brief Uruchamia funkcję do pobierania opisu drogi krajowej.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do pobierania opisu drogi krajowej.
 * Jeśli udało się pobrać ten opis, wypisuje go na standardowe wyjście.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runGetDescription(CmdElement* cmd, Map* map) {
    unsigned routeId;
    CmdElement* tmp;
    char const* description;

    tmp = cmd->nextElement;

    routeId = stringToUnsigned(tmp->input, tmp->inputLength, false);

    description = getRouteDescription(map, routeId);

    if (description != NULL) {
        printf("%s\n", description);
        free((void*) description);
    }

    return description != NULL;
}

/** @brief Uruchamia funkcję do rozszerzania drogi krajowej.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do rozszerzania drogi krajowej.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runExtendRoute(CmdElement* cmd, Map* map) {
    char *cityName;
    unsigned routeId;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    routeId = stringToUnsigned(tmp->input, tmp->inputLength, false);
    tmp = tmp->nextElement;

    cityName = tmp->input;

    return extendRoute(map, routeId, cityName);
}

/** @brief Uruchamia funkcję do usuwania odcinka drogi.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do usuwania odcinka drogi.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runRemoveRoad(CmdElement* cmd, Map* map) {
    char *city1, *city2;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    city1 = tmp->input;
    tmp = tmp->nextElement;

    city2 = tmp->input;

    return removeRoad(map, city1, city2);
}

/** @brief Uruchamia funkcję do tworzenia drogi krajowej.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do tworzenia optymalnej drogi krajowej.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runNewRoute(CmdElement* cmd, Map* map) {
    char *city1, *city2;
    unsigned routeId;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    routeId = stringToUnsigned(tmp->input, tmp->inputLength, false);
    tmp = tmp->nextElement;

    city1 = tmp->input;
    tmp = tmp->nextElement;

    city2 = tmp->input;

    return newRoute(map, routeId, city1, city2);
}

/** @brief Uruchamia funkcję do usuwania drogi krajowej.
 * Ekstrahuje odpowiednie informacje z listy elementów komendy
 * i uruchamia funkcję do usuwania drogi krajowej.
 *
 * @param cmd           - wskaźnik na komendę;
 * @param map           - wskaźnik na mapę;
 * @return Wartość logiczna, odpowiadająca pomyślności wykonania funkcji.
 */
static bool runRemoveRoute(CmdElement* cmd, Map* map) {
    unsigned routeId;
    CmdElement* tmp;

    tmp = cmd->nextElement;

    routeId = stringToUnsigned(tmp->input, tmp->inputLength, false);

    return removeRoute(map, routeId);
}

/** @brief Wywołuje odpowiednią funkcję do obsługi mapy.
 * Wywołuje odpowiednią funkcję do obsługi mapy.
 *
 * @param cmd     - struktura z poleceniem;
 * @param map     - wskaźnik na mapę;
 * @return @p true jeśli się udało, @p false jeśli błąd.
 */
static bool runCmd(CmdElement* cmd, Map* map) {
    bool success;

    success = false;

    switch (cmd->mode) {
        case ROUTE_FROM_DESC_CMD:
            success = runRouteFromDescription(cmd, map);
            break;
        case ADD_ROAD_CMD:
            success = runAddRoad(cmd, map);
            break;
        case REPAIR_ROAD_CMD:
            success = runRepairRoad(cmd, map);
            break;
        case DESCRIPTION_CMD:
            success = runGetDescription(cmd, map);
            break;
        case EXTEND_ROUTE_CMD:
            success = runExtendRoute(cmd, map);
            break;
        case REMOVE_ROAD_CMD:
            success = runRemoveRoad(cmd, map);
            break;
        case NEW_ROUTE_CMD:
            success = runNewRoute(cmd, map);
            break;
        case REMOVE_ROUTE_CMD:
            success = runRemoveRoute(cmd, map);
            break;
        default:
            break;
    }

    return success;
}

bool readAndInterpret(Map* map, MaxNumber* maxU, MaxNumber* maxInt, bool* eofGuard) {
    CmdElement* cmd;
    char c;
    bool success, emptyLine;

    emptyLine = false;

    cmd = readOneElement(&c, BLANK_CMD, eofGuard, &emptyLine);
    if (*eofGuard || emptyLine)
        return true;

    if (cmd == NULL)
        return false;

    if (endOfLine(c)) {
        free(cmd->input);
        free(cmd);
        return false;
    }

    success = recognizeCmd(cmd);
    if (!success) {
        free(cmd->input);
        free(cmd);
        scrollLine(&c);
        return false;
    }

    success = readLine(cmd, maxU, maxInt, &c);
    if (success)
        success = runCmd(cmd, map);
    else
        scrollLine(&c);

    deleteElements(cmd);
    free(cmd->input);
    free(cmd);

    return success;
}

void checkError(bool noError, size_t lineNum) {
    if (!noError)
        fprintf(stderr, "ERROR %zu\n", lineNum);
}