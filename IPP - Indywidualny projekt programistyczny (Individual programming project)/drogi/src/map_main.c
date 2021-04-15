#ifdef NDEBUG
  #undef NDEBUG
#endif

#include "map.h"
#include "helper-strucs.h"
#include "cmd-reader.h"
#include "translators.h"
#include "pre-post.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main() {
    Map* map;
    MaxNumber *maxU, *maxInt;
    bool noError, eof;
    size_t lineCounter;

    if (!pre(&map, &maxU, &maxInt))
        return 0;

    eof = false;
    lineCounter = 0;

    while (!eof) {
        lineCounter++;
        noError = readAndInterpret(map, maxU, maxInt, &eof);
        checkError(noError, lineCounter);
    }

    post(map, maxU, maxInt);

    return 0;
}
