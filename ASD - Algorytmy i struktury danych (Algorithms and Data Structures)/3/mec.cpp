#include <iostream>
#include <cstdint>
#include <algorithm>

using namespace std;

int main() {
    int playerNum, matchNum, whichPlayer, j;

    scanf("%d", &playerNum);
    scanf("%d", &matchNum);

    uint64_t playerIds[playerNum];
    whichPlayer = 0uLL;

    int teamSize = playerNum >> 1;

    for (int i = 0; i < playerNum; i++)
        playerIds[i] = 0;

    for (int i = 0; i < matchNum; i++) {
        for (j = 0; j < teamSize; j++)
            scanf("%d", &whichPlayer);

        for (; j < playerNum; j++) {
            scanf("%d", &whichPlayer);

            playerIds[whichPlayer - 1] = playerIds[whichPlayer - 1] | (1uLL << i);
        }
    }

    sort(playerIds, playerIds+playerNum);

    bool allRivals = true;

    for (int i = 1; i < playerNum && allRivals; i++) {
        if (playerIds[i] == playerIds[i - 1])
            allRivals = false;
    }

    if (allRivals)
        printf("TAK");
    else
        printf("NIE");

    return 0;
}