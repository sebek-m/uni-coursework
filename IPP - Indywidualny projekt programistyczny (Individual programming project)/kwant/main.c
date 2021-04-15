/* Indywidualny Projekt Programistyczny
 * Ma³e zadanie zaliczeniowe - Quantization
 * Autor: Sebastian Miller
 * Data: 22 marca 2019
 *
 * Program opisuje zachowanie cz¹stki w pewnym wyimaginowanym œwiecie kwantowym.
 * Cz¹stka mo¿e przebywaæ w czterech stanach kwantowych symbolizowanych przez
 * liczby ze zbioru {0, 1, 2, 3}.
 * Kwantow¹ histori¹ cz¹stki nazwiemy niepusty ci¹g kolejnyc stanów kwantowych,
 * w jakich cz¹stka przebywa³a. W danym momencie tylko niektóre historie
 * s¹ dopuszczalne. Ka¿da dopuszczalna historia mo¿e mieæ przypisan¹ energiê,
 * ale nie musi. Energiê mo¿na zmieniaæ. Dodatkowo mo¿na zrównaæ
 * energie dwóch historii, tym samym tworz¹c relacjê równowa¿noœci
 * pomiêdzy tymi energiami. W relacji mo¿e byæ wiêcej ni¿ tylko
 * dwie energie. Od momentu zrównania, jeœli energia jednej z historii
 * w relacji zostanie zmieniona, to ta zmiana zajdzie w energiach
 * wszystkich pozosta³ych historii z relacji.
 *
 * Program obs³ugiwany jest za pomoc¹ komend podawanych przez
 * u¿ytkownika na standardowe wejœcie:
 *
 *      DECLARE history - Powoduje, ¿e ka¿da kwantowa historia cz¹stki,
 *                        bêd¹ca prefiksem podanej jako parametr history
 *                        kwantowej historii cz¹stki, staje siê dopuszczalna.
 *      REMOVE history  - Powoduje, ¿e ka¿da kwantowa historia cz¹stki, maj¹ca
 *                        prefiks podany jako parametr history, przestaje
 *                        byæ dopuszczalna.
 *      VALID history   - Sprawdza, czy podana kwantowa historia cz¹stki
 *                        jest dopuszczalna.
 *      ENERGY history
 *             energy   - Jeœli podana w parametrze history kwantowa historia
 *                        cz¹stki jest dopuszczalna i historia ta nie ma przypisanej
 *                        energii, to przypisujejej wartoœæ energii podanej jako
 *                        parametr energy. Jeœli ta kwantowa historia cz¹stki ma
 *                        ju¿ przypisan¹ energiê, to modyfikuje wartoœæ tej energii.
 *      ENERGY history  - Jeœli podana w parametrze history kwantowa historia cz¹stki
 *                        jest dopuszczalna i historia ta ma przypisan¹ energiê,
 *                        to wypisuje wartoœæ tej energii.
 *      EQUAL history_a
 *            history_b - Jeœli podane w parametrach history_a i history_b kwantowe
 *                        historie cz¹stki s¹ dopuszczalne, to: (a) jeœli chocia¿
 *                        jedna z nich ma przypisan¹ energiê, zrównuje ich energie
 *                        i domyka relacjê równowa¿noœci; (b) jeœli obie historie
 *                        przed wykonaniem tego polecenia maj¹ przypisane energie,
 *                        to po wykonaniu polecenia energia przypisana tym historiom
 *                        jest œredni¹ arytmetyczn¹ tych energii (z ewentualnym
 *                        zaokr¹gleniem w dó³); (c) jeœli obie historie s¹ identyczne
 *                        lub maj¹ ju¿ zrównane energie, niczego nie zmienia.
 *
 * Program wypisuje odpowiednie informacje zwi¹zane z wywo³an¹ komend¹ lub, w przypadku
 * b³êdnej komendy lub próby wywo³ania komendy z nieprawid³owymi danymi, wiersz z napisem
 * ERROR na standardowe wyjœcie diagnostyczne.
 * Dopuszczalne energie to liczby ca³kowite mieszcz¹ce siê w zakresie 1 - (2^64 - 1).
 * Dlatego te¿ w programie skorzystano z biblioteki <inttypes.h>,
 * w której zaimplementowany jest typ uint64_t, który przeznacza 64 bity
 * na liczbê w nim przechowywan¹, niezale¿nie od maszyny, a dok³adnie tylu
 * bitów potrzeba do zmieszczenia liczby 2^64 - 1. U¿ycie unsigned long long int
 * mog³oby poskutkowaæ brakiem funkcjonalnoœci programu na maszynach 32-bitowych.
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
    //  root            - wskaŸnik na korzeñ drzewa z dopuszczalnymi
    //                    historiami
    //  continueProgram - stra¿nik pêtli wykonywania programu
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
