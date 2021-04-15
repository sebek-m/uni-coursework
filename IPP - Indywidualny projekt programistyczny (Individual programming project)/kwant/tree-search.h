#include "structure.h"

#ifndef TREE_SEARCH_H
#define TREE_SEARCH_H

extern void treeIterationStep(History* current, char direction);

extern void findHistory(History* hisTree, char* testedHistory, int testedHistoryLength);

#endif /* TREE_SEARCH_H */

