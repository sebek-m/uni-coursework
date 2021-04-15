#include <stdbool.h>
#include "structure.h"

#ifndef CMD_READER_H
#define CMD_READER_H

extern int recognizeCommand();

extern void runCommand(int cmd, History hisTree, int* maxEnergyArray,
                       int maxEnergyLength, bool* continueProgram);

#endif /* CMD_READER_H */
