/* Indywidualny Projekt Programistyczny
 * Ma�e zadanie zaliczeniowe - Quantization
 * Autor: Sebastian Miller
 * Data: 22 marca 2019
 *
 * Program opisuje zachowanie cz�stki w pewnym wyimaginowanym �wiecie kwantowym.
 * Cz�stka mo�e przebywa� w czterech stanach kwantowych symbolizowanych przez
 * liczby ze zbioru {0, 1, 2, 3}.
 * Kwantow� histori� cz�stki nazwiemy niepusty ci�g kolejnyc stan�w kwantowych,
 * w jakich cz�stka przebywa�a. W danym momencie tylko niekt�re historie
 * s� dopuszczalne. Ka�da dopuszczalna historia mo�e mie� przypisan� energi�,
 * ale nie musi. Energi� mo�na zmienia�. Dodatkowo mo�na zr�wna�
 * energie dw�ch historii, tym samym tworz�c relacj� r�wnowa�no�ci
 * pomi�dzy tymi energiami. W relacji mo�e by� wi�cej ni� tylko
 * dwie energie. Od momentu zr�wnania, je�li energia jednej z historii
 * w relacji zostanie zmieniona, to ta zmiana zajdzie w energiach
 * wszystkich pozosta�ych historii z relacji.
 *
 * Program obs�ugiwany jest za pomoc� komend podawanych przez
 * u�ytkownika na standardowe wej�cie:
 *
 *      DECLARE history - Powoduje, �e ka�da kwantowa historia cz�stki,
 *                        b�d�ca prefiksem podanej jako parametr history
 *                        kwantowej historii cz�stki, staje si� dopuszczalna.
 *      REMOVE history  - Powoduje, �e ka�da kwantowa historia cz�stki, maj�ca
 *                        prefiks podany jako parametr history, przestaje
 *                        by� dopuszczalna.
 *      VALID history   - Sprawdza, czy podana kwantowa historia cz�stki
 *                        jest dopuszczalna.
 *      ENERGY history
 *             energy   - Je�li podana w parametrze history kwantowa historia
 *                        cz�stki jest dopuszczalna i historia ta nie ma przypisanej
 *                        energii, to przypisujejej warto�� energii podanej jako
 *                        parametr energy. Je�li ta kwantowa historia cz�stki ma
 *                        ju� przypisan� energi�, to modyfikuje warto�� tej energii.
 *      ENERGY history  - Je�li podana w parametrze history kwantowa historia cz�stki
 *                        jest dopuszczalna i historia ta ma przypisan� energi�,
 *                        to wypisuje warto�� tej energii.
 *      EQUAL history_a
 *            history_b - Je�li podane w parametrach history_a i history_b kwantowe
 *                        historie cz�stki s� dopuszczalne, to: (a) je�li chocia�
 *                        jedna z nich ma przypisan� energi�, zr�wnuje ich energie
 *                        i domyka relacj� r�wnowa�no�ci; (b) je�li obie historie
 *                        przed wykonaniem tego polecenia maj� przypisane energie,
 *                        to po wykonaniu polecenia energia przypisana tym historiom
 *                        jest �redni� arytmetyczn� tych energii (z ewentualnym
 *                        zaokr�gleniem w d�); (c) je�li obie historie s� identyczne
 *                        lub maj� ju� zr�wnane energie, niczego nie zmienia.
 *
 * Program wypisuje odpowiednie informacje zwi�zane z wywo�an� komend� lub, w przypadku
 * b��dnej komendy lub pr�by wywo�ania komendy z nieprawid�owymi danymi, wiersz z napisem
 * ERROR na standardowe wyj�cie diagnostyczne.
 * Dopuszczalne energie to liczby ca�kowite mieszcz�ce si� w zakresie 1 - (2^64 - 1).
 * Dlatego te� w programie skorzystano z biblioteki <inttypes.h>,
 * w kt�rej zaimplementowany jest typ uint64_t, kt�ry przeznacza 64 bity
 * na liczb� w nim przechowywan�, niezale�nie od maszyny, a dok�adnie tylu
 * bit�w potrzeba do zmieszczenia liczby 2^64 - 1. U�ycie unsigned long long int
 * mog�oby poskutkowa� brakiem funkcjonalno�ci programu na maszynach 32-bitowych.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "structure.h"
#include "pre-post.h"
#include "cmd-reader.h"

int main() {
    //  cmd             - odpowiada za kody komend
    //  maxEnergyArray  - tablica z cyframi liczby 2^54 -1
    //                    (maksymalnej energii)
    //  maxEnergyLength - rozmiar tej tablicy
    //  root            - wska�nik na korze� drzewa z dopuszczalnymi
    //                    historiami
    //  continueProgram - stra�nik p�tli wykonywania programu
    int cmd, *maxEnergyArray, maxEnergyLength;
    History root;
    bool continueProgram;

    // 'r' to nie poprawny stan kwantowy,
    // ale w korzeniu nie ma to znaczenia
    root = newNode('r');
    prepareMaxEnergyArray(&maxEnergyArray, &maxEnergyLength);
    continueProgram = true;

    while (continueProgram) {
        cmd = recognizeCommand();
        runCommand(cmd, root, maxEnergyArray,
                   maxEnergyLength, &continueProgram);
    }

    deleteTreeFinal(root);
    free(maxEnergyArray);

    return 0;
}
