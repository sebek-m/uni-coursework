#include <stdbool.h>

#ifndef UTILS_H
#define UTILS_H

extern void error();

extern bool endOfLine(char c);

extern void scrollLine(char* c);

extern bool noHistoryGiven(char c, int strLen);

extern void addCharToString(char** array, char* c, int* i, int* size);

extern bool correctQuantumState(char state);

extern void prepareTemporaryHistoryArray(char** tmpHistory, int* tmpHistoryLength, char* c);

#endif /* UTILS_H */

#ifndef INITIAL_STRING_SIZE
#define INITIAL_STRING_SIZE 5
#endif /* INITIAL_STRING_SIZE */
