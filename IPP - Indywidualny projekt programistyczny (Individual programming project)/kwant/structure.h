/* Deklracja struktury bêd¹cej drzewem dopuszczalnych historii.
 * Ka¿dy wêze³ odpowiada za jeden stan w danej historii,
 * ale jednoczeœnie reprezentuje ca³¹ historiê, poniewa¿
 * œcie¿ka prowadz¹ca do niego od korzenia mo¿e byc zinterpretowana
 * jako w³aœnie pe³na historia. Np. historia 0123, bêdzie reprezentowana
 * przez wêze³ na koñcu œcie¿ki (((root->zero)->one)->two)->three.
 * Dodatkowo w strukturze s¹ dowi¹zania do poprzednika i nastêpnika
 * w liœcie relacji do której nale¿y energia danej historii.
 *
 * Pola struktury:
 *  - value         - znak reprezentuj¹cy stan kwantowy w historii
 *  - energy        - wskaŸnik na zmienn¹ z wartoœci¹ energii przypisanej
 *                    do danej historii
 *  - next, prev    - wskaŸniki na nastêpnik i poprzednik w liœcie relacji
 *  - zero, one
 *    two, three    - wskaŸniki na odpowiednich synów, którzy odpowiadaj¹
 *                    kolejnym stanom w jakiejœ historii
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
