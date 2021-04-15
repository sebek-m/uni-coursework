#include <iostream>

using namespace std;

#define BILLION 1000000000

int howManyLeaves(int elNum) {
    int leaveNum = 1;

    while (leaveNum < elNum)
        leaveNum *= 2;

    return leaveNum;
}

bool isLeftSon(int treeInd) {
    return treeInd % 2 == 1;
}

int analogousRight(int treeInd) {
    return treeInd + 1;
}

int father(int treeInd) {
    if (treeInd == 0)
        return -1;

    return (treeInd - 1) / 2;
}

unsigned long int howManyInversions(int k, unsigned long int intervalTree[], int treeSize,
                                     const unsigned long int perm[], unsigned long int prevInv[],
                                     int elNum) {
    int lastNonLeave;

    lastNonLeave = treeSize / 2 - 1;

    fill(prevInv, prevInv + elNum, 1);

    for (int l = 2; l <= k; l++) {
        fill(intervalTree, intervalTree + treeSize, 0);

        for (int el = 0; el < elNum; el++) {
            int treeInd = lastNonLeave + perm[el];
            unsigned long int lInversions = 0;

            while (treeInd >= 0) {
                unsigned long int currentVal = intervalTree[treeInd];
                intervalTree[treeInd] = (currentVal + prevInv[el]) % BILLION;

                if (isLeftSon(treeInd))
                    lInversions = (lInversions + intervalTree[analogousRight(treeInd)]) % BILLION;

                treeInd = father(treeInd);
            }

            prevInv[el] = lInversions;
        }
    }

    unsigned long int sum = 0;
    for (int i = 0; i < elNum; i++) {
        sum = (sum + prevInv[i]) % BILLION;
    }

    return sum;
}

int main() {
    int numOfEl, k, leaveNum, treeSize;

    scanf("%d", &numOfEl);
    scanf("%d", &k);

    unsigned long int perm[numOfEl];
    unsigned long int prevInv[numOfEl];

    for (int i = 0; i < numOfEl; i++)
        scanf("%d", &(perm[i]));

    leaveNum = howManyLeaves(numOfEl);
    treeSize = 2 * leaveNum - 1;

    unsigned long int intervalTree[treeSize];

    printf("%d", howManyInversions(k, intervalTree, treeSize, perm, prevInv, numOfEl));

    return 0;
}