#include <iostream>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <vector>
#include <tuple>
#include <queue>
#include <iterator>

using namespace std;

int islandNo;

int distance(const tuple<int, int, int> &p1, const tuple<int, int, int> &p2) {
	return min(abs(get<0>(p1) - get<0>(p2)), abs(get<1>(p1) - get<1>(p2)));
}

void addToGraph(tuple<int, int, int> sorted[], vector<pair<int, int>> graph[]) {
	for (int i = 0; i < islandNo; i++) {
		tuple<int, int, int> currIsl = sorted[i];

		if (i > 0) {
			tuple<int, int, int> leftNbh = sorted[i - 1];

			graph[get<2>(currIsl)].push_back(make_pair(distance(currIsl, leftNbh), get<2>(leftNbh)));
		}

		if (i < islandNo - 1) {
			tuple<int, int, int> rightNbh = sorted[i + 1];

			graph[get<2>(currIsl)].push_back(make_pair(distance(currIsl, rightNbh), get<2>(rightNbh)));
		}
	}
}

bool sortByY(const tuple<int, int, int> &p1, const tuple<int, int, int> &p2) { 
    return get<1>(p1) < get<1>(p2); 
} 

int main() {
	scanf("%d", &islandNo);

	tuple<int, int, int> islands[islandNo];

	for (int i = 0; i < islandNo; i++) {
		int x, y;

		scanf("%d %d", &x, &y);

		islands[i] = make_tuple(x, y, i);
	}

	tuple<int, int, int> xSorted[islandNo];
	tuple<int, int, int> ySorted[islandNo];

	copy(islands, islands + islandNo, xSorted);
	copy(islands, islands + islandNo, ySorted);

	sort(xSorted, xSorted + islandNo);
	sort(ySorted, ySorted + islandNo, sortByY);

	// first - odleglosc/waga, second - nr wyspy
	// 0 - wsch, 1 - zach, 2 - płd, 3 - płn - to raczej nie
	vector<pair<int, int>> graph[islandNo];

	/*for (int i = 0; i < islandNo; i++)
		graph.[i].reserve(4);*/

	addToGraph(xSorted, graph);
	addToGraph(ySorted, graph);

	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> Q;

	vector<int> distFromSource(islandNo, INT_MAX);

	Q.push(make_pair(0, 0));
	distFromSource[0] = 0;

	while (!Q.empty()) {
		int v = Q.top().second;
		Q.pop();

		vector<pair<int, int>> adjacent = graph[v];

		for (pair<int, int> nbh : adjacent) {
			int nbhId = nbh.second;
			int weight = nbh.first;

			if (distFromSource[v] + weight < distFromSource[nbhId]) {
				distFromSource[nbhId] = distFromSource[v] + weight;
				Q.push(make_pair(distFromSource[nbhId], nbhId));
			}
		}
	}

	printf("%d", distFromSource[islandNo - 1]);
}