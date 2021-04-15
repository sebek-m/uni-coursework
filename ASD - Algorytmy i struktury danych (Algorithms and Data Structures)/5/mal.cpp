#include <iostream>
#include <set>
#include <utility>

using namespace std;

int main() {
    int length, duration;

    scanf("%d", &length);
    scanf("%d", &duration);

    set<pair<int, int>> painted;

    int whiteSum = 0;

    for (int day = 0; day < duration; day++) {
        int end1, end2;
        char mode;

        scanf("%d", &end1);
        scanf("%d", &end2);
        scanf(" %c", &mode);

        pair<int, int> seg = {end1, end2};

        if (mode == 'B') {
            auto it = painted.lower_bound(seg);

            bool added = false;

            if (it == painted.end()) {
                auto tmpIt = it;

                if (it == painted.begin() || (*(--tmpIt)).second < end1) {
                    painted.insert(it, seg);
                    whiteSum += end2 - end1 + 1;
                    added = true;
                }
            }

            if (!added) {
                auto tmpIt = it;

                if (tmpIt == painted.end())
                    mode = 'i';

                if (it != painted.begin() && (*(--tmpIt)).second >= end1)
                    it = tmpIt;

                if (!((*it).first < end1 && (*it).second > end2)) {
                    if ((*it).first < end1)
                        end1 = (*it).first;

                    while (it != painted.end() && (*it).second < end2) {
                        whiteSum -= (*it).second - (*it).first + 1;

                        auto tmpIt = it;
                        tmpIt++;
                        painted.erase((*it));
                        it = tmpIt;
                    }

                    if (it != painted.end() && (*it).first <= end2) {
                        end2 = (*it).second;

                        whiteSum -= (*it).second - (*it).first + 1;
                        painted.erase((*it));
                    }

                    seg = {end1, end2};

                    painted.insert(seg);

                    whiteSum += end2 - end1 + 1;
                }
            }
        }
        else { // mode == 'C'
            auto it = painted.lower_bound(seg);

            bool enter = false;

            if (it == painted.end()) {
                auto tmpIt = it;

                if (it != painted.begin() && (*(--tmpIt)).second >= end1)
                    enter = true;
            }
            else {
                enter = true;
            }

            if (enter) {
                auto tmpIt = it;

                if (it != painted.begin() && (*(--tmpIt)).second >= end1)
                    it--;

                if ((*it).first < end1 && (*it).second > end2) {
                    int tmpStart = (*it).first;
                    int tmpEnd = end1 - 1;

                    int tmpStart2 = end2 + 1;
                    int tmpEnd2 = (*it).second;

                    whiteSum -= end2 - end1 + 1;

                    auto tmpIt = it;
                    tmpIt++;

                    painted.erase((*it));

                    pair<int, int> seg1 = {tmpStart, tmpEnd};
                    pair<int, int> seg2 = {tmpStart2, tmpEnd2};

                    painted.insert(tmpIt, seg1);
                    painted.insert(++tmpIt, seg2);
                }
                else {
                    if ((*it).first < end1) {
                        whiteSum -= (*it).second - end1 + 1;

                        int tmpStart = (*it).first;
                        int tmpEnd = end1 - 1;

                        pair<int, int> shortened = {tmpStart, tmpEnd};

                        auto tmpIt = it;
                        tmpIt++;

                        painted.erase((*it));
                        painted.insert(tmpIt, shortened);
                        it = tmpIt;
                    }

                    while (it != painted.end() && (*it).second < end2) {
                        whiteSum -= (*it).second - (*it).first + 1;

                        auto tmpIt = it;
                        tmpIt++;
                        painted.erase((*it));
                        it = tmpIt;
                    }

                    if (it != painted.end() && (*it).first <= end2) {
                        whiteSum -= end2 - (*it).first + 1;

                        int tmpStart = end2 + 1;
                        int tmpEnd = (*it).second;

                        auto tmpIt = it;
                        tmpIt++;

                        painted.erase((*it));

                        if (tmpStart <= tmpEnd) {
                            pair<int, int> shortened = {tmpStart, tmpEnd};
                            painted.insert(tmpIt, shortened);
                        }
                    }
                }
            }
        }

        printf("%d\n", whiteSum);
    }
    return 0;
}