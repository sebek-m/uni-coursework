#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

const int BILLION = 1000000000;

void fillStructures(pair<int, int> branchTypes[], int sums[], int levelStatus[], const int& n) {
    fill(levelStatus, levelStatus + n + 1, 0);
    // Ustawiamy korzeń
    levelStatus[n] = 1;

    sums[0] = 1;

    for (int i = 1; i <= n; i++) {
        int a, b, leftSum, rightSum, currentSum;

        scanf("%d %d", &a, &b);

        branchTypes[i] = make_pair(a, b);

        leftSum = sums[a];
        rightSum = sums[b];

        currentSum = (leftSum + rightSum + 1) % BILLION;

        sums[i] = currentSum;
    }
}

int howManyApples(const int& n, const int& d, const pair<int, int> branchTypes[],
                  const int sums[], int levelStatus[]) {
    auto* previousStatus = new int[n + 1];

    for (int lvl = 1; lvl < d; lvl++) {
        copy(levelStatus, levelStatus + n + 1, previousStatus);

        for (int i = 1; i <= n; i++) {
            if (previousStatus[i] > 0) {
                int leftType = branchTypes[i].first;
                int rightType = branchTypes[i].second;

                levelStatus[leftType] = (levelStatus[leftType] + previousStatus[i]) % BILLION;
                levelStatus[rightType] = (levelStatus[rightType] + previousStatus[i]) % BILLION;

                levelStatus[i] = (levelStatus[i] - previousStatus[i]) % BILLION;
            }
        }
    }

    int sum = 0;

    for (int i = 1; i <= n; i++) {
        if (levelStatus[i] > 0) {
            int leftType = branchTypes[i].first;
            int rightType = branchTypes[i].second;

            int leftSum = sums[leftType];
            int rightSum = sums[rightType];

            sum = (sum + 1ULL * levelStatus[i] * leftSum +
                    1ULL * levelStatus[i] * rightSum) % BILLION;
        }

    }

    delete[] previousStatus;

    return sum;
}

int main() {
    int n, d;

    scanf("%d %d", &n, &d);

    auto* branchTypes = new pair<int, int>[n + 1];
    auto* sums = new int[n + 1];
    auto* levelStatus = new int[n + 1];

    fillStructures(branchTypes, sums, levelStatus, n);

    int result = howManyApples(n, d, branchTypes, sums, levelStatus);

    printf("%d", result);

    delete[] branchTypes;
    delete[] sums;
    delete[] levelStatus;

    return 0;
}