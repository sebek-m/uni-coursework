/* Deklracja struktury b�d�cej drzewem dopuszczalnych historii.
 * Ka�dy w�ze� odpowiada za jeden stan w danej historii,
 * ale jednocze�nie reprezentuje ca�� histori�, poniewa�
 * �cie�ka prowadz�ca do niego od korzenia mo�e byc zinterpretowana
 * jako w�a�nie pe�na historia. Np. historia 0123, b�dzie reprezentowana
 * przez w�ze� na ko�cu �cie�ki (((root->zero)->one)->two)->three.
 * Dodatkowo w strukturze s� dowi�zania do poprzednika i nast�pnika
 * w li�cie relacji do kt�rej nale�y energia danej historii.
 *
 * Pola struktury:
 *  - value         - znak reprezentuj�cy stan kwantowy w historii
 *  - energy        - wska�nik na zmienn� z warto�ci� energii przypisanej
 *                    do danej historii
 *  - next, prev    - wska�niki na nast�pnik i poprzednik w li�cie relacji
 *  - zero, one
 *    two, three    - wska�niki na odpowiednich syn�w, kt�rzy odpowiadaj�
 *                    kolejnym stanom w jakiej� historii
 */

#include <inttypes.h>

#ifndef STRUCTURE_H
#define STRUCTURE_H

struct State;

typedef struct State* History;

struct State {
  char value;
  uint64_t* energy;
  History next, prev;
  History zero, one, two, three;
};

extern History newNode(char state);

#endif /* STRUCTURE_H */
