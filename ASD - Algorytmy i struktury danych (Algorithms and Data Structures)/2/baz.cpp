#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>

using namespace std;

bool possible(int prodNo, int k, const int* lowest, const int* highest) {
    return lowest[prodNo - k] != -1 &&
            highest[prodNo - k] != -1;
}

void fillHighest(int prodNo, const int* prices, bool even, int* highest) {
    int pos, temp;

    pos = 0;
    temp = -1;

    while (pos < prodNo) {
        for (; pos < prodNo && (prices[pos] % 2 == 0) != even; pos++)
            highest[pos] = temp;

        if (pos < prodNo) {
            highest[pos] = temp;
            temp = prices[pos];
            pos++;
        }
    }
}

void fillLowest(int prodNo, const int* prices, bool even, int* lowest) {
    int pos = prodNo - 1;
    int temp = -1;

    while (pos >= 0) {
        for (; pos >= 0; pos--) {
            if ((prices[pos] % 2 == 0) == even)
                temp = prices[pos];

            lowest[pos] = temp;
        }
    }
}

long long int correctSum(long long int* prefixSums, int* lowestEven,
                         int* lowestOdd, int* highestEven, int* highestOdd,
                         int k, int productNo) {
    bool option1 = possible(productNo, k, lowestEven, highestOdd);
    bool option2 = possible(productNo, k, lowestOdd, highestEven);

    if (option1 && option2)
        return max(prefixSums[k - 1] - lowestEven[productNo - k] + highestOdd[productNo - k],
               prefixSums[k - 1] - lowestOdd[productNo - k] + highestEven[productNo - k]);
    else if (option1)
        return prefixSums[k - 1] - lowestEven[productNo - k] + highestOdd[productNo - k];
    else if (option2)
        return prefixSums[k - 1] - lowestOdd[productNo - k] + highestEven[productNo - k];
    else
        return -1;
}

int main() {
    ios_base::sync_with_stdio(0);

    int productNo, daysNo;

    scanf("%d", &productNo);

    int prices[productNo];

    for (int i = 0; i < productNo; i++)
        scanf("%d", &(prices[i]));

    scanf("%d", &daysNo);

    int kDays[daysNo];

    for (int i = 0; i < daysNo; i++)
        scanf("%d", &(kDays[i]));

    long long int prefixSums[productNo];
    prefixSums[0] = prices[productNo - 1];

    for (int j = 1; j < productNo; j++)
        prefixSums[j] = prefixSums[j - 1] + prices[productNo - 1 - j];

    int highestEven[productNo];
    fillHighest(productNo, prices, true, highestEven);

    int highestOdd[productNo];
    fillHighest(productNo, prices, false, highestOdd);

    int lowestEven[productNo];
    fillLowest(productNo, prices, true, lowestEven);

    int lowestOdd[productNo];
    fillLowest(productNo, prices, false, lowestOdd);

    ostringstream finalMoney;

    for (int i = 0; i < daysNo; i++) {
        int k = kDays[i];

        if (prefixSums[k - 1] % 2 != 0) {
            finalMoney << prefixSums[k - 1];
        }
        else {
            finalMoney << correctSum(prefixSums, lowestEven, lowestOdd,
                                     highestEven, highestOdd, k, productNo);
        }

        if (i != daysNo - 1)
            finalMoney << '\n';
    }

    cout << finalMoney.str();

    return 0;
}