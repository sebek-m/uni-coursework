#include <iostream>

using namespace std;

int main() {
    int length, temp;

    scanf("%d", &length);

    int numbers[length];

    for (int i = 0; i < length; i++) {
        scanf("%d", &temp);
        numbers[i] = temp;
    }

    unsigned long long int power = 1000000000;

    unsigned long long int permNo[length][length][2];

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            if (i == j) {
                permNo[i][j][0] = 1;
                permNo[i][j][1] = 0;
            }
            else {
                permNo[i][j][0] = 0;
                permNo[i][j][1] = 0;
            }
        }
    }

    for (int j = 1; j < length; j++) {
        for (int i = j - 1; i >= 0; i--) {
            if (numbers[i] < numbers[i + 1])
                permNo[i][j][0] = (permNo[i][j][0] + permNo[i + 1][j][0]) % power;

            if (numbers[i] < numbers[j]) {
                permNo[i][j][0] = (permNo[i][j][0] + permNo[i + 1][j][1]) % power;
                permNo[i][j][1] = (permNo[i][j][1] + permNo[i][j - 1][0]) % power;
            }

            if (numbers[j - 1] < numbers[j])
                permNo[i][j][1] = (permNo[i][j][1] + permNo[i][j - 1][1]) % power;
        }
    }

    cout << (permNo[0][length - 1][0] + permNo[0][length - 1][1]) % power;
    return 0;
}
