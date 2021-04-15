#include <iostream>
#include <vector>
#include <climits>
#include <cstdio>

using namespace std;

int templateLength(int wallpaperLength, int maxLength) {
    return wallpaperLength - maxLength + 1;
}

vector<char> rewrite() {
    char c;
    vector<char> pattern;

    c = getchar();

    while (c != EOF && c != '\n') {
        pattern.push_back(c);
        c = getchar();
    }

    return pattern;
}

int calculateTemplate2() {
    vector <char> pattern;
    int i, j, distance, minDistance;

    pattern = rewrite();
    if (pattern.empty())
        return 0;

    i = 0;

    while (i < pattern.size() && pattern[i] == '*')
        i++;

    if (i == pattern.size())
        return 1;

    j = i + 1;
    minDistance = pattern.size();

    while (j < pattern.size()) {
        while (j < pattern.size() && (pattern[j] == pattern[i] || pattern[j] == '*')) {
            if (pattern[i] == pattern[j])
                i = j;

            j++;
        }

        if (j < pattern.size()) {
            distance = j - i;
            i = j;

            if (distance < minDistance)
                minDistance = distance;
        }
    }

    return templateLength(pattern.size(), minDistance);
}

int main() {
    ios_base::sync_with_stdio(0);

    cout << calculateTemplate2();

    return 0;
}