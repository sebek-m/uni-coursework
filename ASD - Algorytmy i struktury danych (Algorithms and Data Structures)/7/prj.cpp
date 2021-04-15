#include <iostream>
#include <queue>
#include <utility>
#include <list>

using namespace std;

int main() {
    int projectNum, edgeNum, k;

    scanf("%d %d %d", &projectNum, &edgeNum, &k);

    int programmersNeeded[projectNum];

    for (int i = 0; i < projectNum; i++)
        scanf("%d", &(programmersNeeded[i]));

    pair<vector<int>, vector<int>> graph[projectNum];

    for (int i = 0; i < edgeNum; i++) {
        int prjId, needs;

        scanf("%d %d", &prjId, &needs);

        graph[prjId - 1].first.push_back(needs - 1);
        graph[needs - 1].second.push_back(prjId - 1);
    }

    int outDeg[projectNum];
    priority_queue<pair<int, int>, vector<pair<int,int>>, greater<>> Q;

    for (int i = 0; i < projectNum; i++) {
        outDeg[i] = graph[i].first.size();

        if (outDeg[i] == 0)
            Q.push(make_pair(programmersNeeded[i], i));
    }

    list<pair<int, int>> OUT;

    while (!Q.empty() && OUT.size() < k) {
        pair<int, int> v = Q.top();
        Q.pop();
        OUT.push_back(v);

        for(int u : graph[v.second].second) {
            outDeg[u]--;
            if (outDeg[u] == 0)
                Q.push(make_pair(programmersNeeded[u], u));
        }
    }

    int maxProgrammersNeeded = 0;

    for (pair<int, int> vertex : OUT) {
        if (vertex.first > maxProgrammersNeeded)
            maxProgrammersNeeded = vertex.first;
    }

    printf("%d", maxProgrammersNeeded);

    return 0;
}