#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>
#include <tuple>
#include <climits>

using namespace std;

int wordLength, queryNo, subWordsNo, *powers, **names;
string word;

void evaluateQuery(const int query[]) {
    int length1, length2, k;
    pair<int, int> first, second;

    length1 = query[1] - query[0] + 1;
    length2 = query[3] - query[2] + 1;

    int minLength = min(length1, length2);

    for (k = 0; k < subWordsNo && powers[k] < minLength; k++);

    if (k == subWordsNo || powers[k] > minLength)
        k--;

    if (powers[k] == minLength)
        first = make_pair(names[k][query[0] - 1], names[k][query[0] - 1]);
    else {
        int bPos = query[0] + minLength - powers[k] - 1;
        first = make_pair(names[k][query[0] - 1], names[k][bPos]);
    }

    if (powers[k] == minLength)
        second = make_pair(names[k][query[2] - 1], names[k][query[2] - 1]);
    else {
        int bPos = query[2] + minLength - powers[k] - 1;
        second = make_pair(names[k][query[2] - 1], names[k][bPos]);
    }

    if (length1 == length2) {
        if (first == second)
            printf("=\n");
        else if (first < second)
            printf("<\n");
        else
            printf(">\n");
    }
    else if (length1 < length2) {
        if (first == second || first < second)
            printf("<\n");
        else
            printf(">\n");
    }
    else {
        if (first == second || first > second)
            printf(">\n");
        else
            printf("<\n");
    }
}

int main() {
	scanf("%d %d", &wordLength, &queryNo);

	cin >> word;

	int** queries = new int*[queryNo];

	for (int i = 0; i < queryNo; i++) {
		queries[i] = new int[4];

		scanf("%d %d %d %d", &queries[i][0], &queries[i][1], &queries[i][2], &queries[i][3]);
	}

	subWordsNo = floor(log2(wordLength));
	subWordsNo++;
	names = new int*[subWordsNo];

	for (int i = 0; i < subWordsNo; i++)
		names[i] = new int[wordLength];

	powers = new int[subWordsNo];
	powers[0] = 1;
	for (int i = 1; i < subWordsNo; i++)
        powers[i] = 2 * powers[i - 1];

    tuple<int, int, int> composition0[wordLength];
    for (int i = 0; i < wordLength; i++)
        composition0[i] = make_tuple(word[i], word[i], i);

    sort(composition0, composition0 + wordLength);

    int pos = 1;
    names[0][get<2>(composition0[0])] = pos;

    for (int i = 1; i < wordLength; i++) {
        if (get<0>(composition0[i]) != get<0>(composition0[i - 1]) ||
            get<1>(composition0[i]) != get<1>(composition0[i - 1]))
            pos++;

        names[0][get<2>(composition0[i])] = pos;
    }

	for (int i = 1; i < subWordsNo; i++) {
		tuple<int, int, int> composition[wordLength];

		int j;

		for (j = 0; j < wordLength - powers[i] + 1; j++)
			composition[j] = make_tuple(names[i - 1][j], names[i - 1][j + powers[i - 1]], j);

		for (; j < wordLength; j++)
			composition[j] = make_tuple(INT_MAX, INT_MAX, j);

		sort(composition, composition + wordLength);

		j = 1;
		pos = 1;
		names[i][get<2>(composition[0])] = pos;

		while (j < wordLength) {
		    if (get<0>(composition[j]) != get<0>(composition[j - 1]) ||
                    get<1>(composition[j]) != get<1>(composition[j - 1]))
		        pos++;

		    names[i][get<2>(composition[j])] = pos;

		    j++;
		}
	}

	for (int i = 0; i < queryNo; i++)
	    evaluateQuery(queries[i]);

	for (int i = 0; i < queryNo; i++)
		delete[] queries[i];

	for (int i = 0; i < subWordsNo; i++)
		delete[] names[i];

	delete[] queries;
	delete[] names;
	delete[] powers;

	return 0;
}