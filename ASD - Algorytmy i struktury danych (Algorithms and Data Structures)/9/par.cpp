#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

int gladeNo;
int height;
int pow2;

int ancestor(int v, int distance, int** link) {
    int anc = v;
    int powOf2 = pow2;
    int i = height;

    while (distance > 0) {
        if (powOf2 > distance) {
            i--;
            powOf2 /= 2;
        }
        else {
            anc = link[i][anc];
            distance -= powOf2;
        }
    }

    return anc;
}

class Tree {
private:
    void fillFarDown(pair<int, int> fardown[], int i) {
        pair<int, int> cand[3];

        pair<int, int> nonExistent = make_pair(-1, i);

        cand[1] = nonExistent;
        cand[2] = nonExistent;

        if (left[i] != -1) {
            fillFarDown(fardown, left[i]);
            cand[1] = make_pair(fardown[left[i]].first + 1, fardown[left[i]].second);
        }

        if (right[i] != -1) {
            fillFarDown(fardown, right[i]);
            cand[2] = make_pair(fardown[right[i]].first + 1, fardown[right[i]].second);
        }

        cand[0] = make_pair(0, i);

        fardown[i] = *max_element(cand, cand + 3);
    }

    void fillFarUp(pair<int, int> farup[], pair<int, int> fardown[], int i) {
        pair<int, int> cand[3];

        pair<int, int> nonExistent = make_pair(-1, i);

        cand[1] = nonExistent;
        cand[2] = nonExistent;

        cand[0] = make_pair(0, i);

        if (parent[i] != -1)
            cand[1] = make_pair(farup[parent[i]].first + 1, farup[parent[i]].second);

        if (sibling[i] != -1)
            cand[2] = make_pair(fardown[sibling[i]].first + 2, fardown[sibling[i]].second);

        farup[i] = *max_element(cand, cand + 3);

        if (left[i] != -1)
            fillFarUp(farup, fardown, left[i]);

        if (right[i] != -1)
            fillFarUp(farup, fardown, right[i]);
    }
public:
    int* left;
    int* right;
    int* parent;
    int* sibling;

    explicit Tree(int n) {
        left = new int[n];
        right = new int[n];
        parent = new int[n];
        sibling = new int[n];

        for (int i = 0; i < n; i++) {
            parent[i] = -1;
            sibling[i] = -1;
        }
    }

    ~Tree() {
        delete[] left;
        delete[] right;
        delete[] parent;
        delete[] sibling;
    }

    void fillDepth(int depth[], int i, int currDepth) {
        depth[i] = currDepth;

        if (left[i] != -1)
            fillDepth(depth, left[i], currDepth + 1);

        if (right[i] != -1)
            fillDepth(depth, right[i], currDepth + 1);
    }

    int lowestCommonAncestor(int u, int v, const int depth[], int** link) const {
        int uDepth = depth[u];
        int vDepth = depth[v];

        if (uDepth < vDepth)
            v = ancestor(v, vDepth - uDepth, link);
        else if (uDepth > vDepth)
            u = ancestor(u, uDepth - vDepth, link);

        if (u == v)
            return v;

        int i = height;

        while (i > 0) {
            if (link[i - 1][u] != link[i - 1][v]) {
                u = link[i - 1][u];
                v = link[i - 1][v];
            }

            i--;
        }

        return parent[u];
    }

    void fillFar(pair<int, int>* far) {
        pair<int, int> fardown[gladeNo], farup[gladeNo];

        fillFarDown(fardown, 0);
        fillFarUp(farup, fardown, 0);

        for (int i = 0; i < gladeNo; i++)
            far[i] = fardown[i] > farup[i] ? fardown[i] : farup[i];
    }
};

int answerQuery(pair<int, int> query, const int depth[], const pair<int, int> far[], const Tree& tree, int** link) {
    pair<int, int> farthest = far[query.first];

    if (query.second > farthest.first)
        return -1;

    int lowestCommon = tree.lowestCommonAncestor(query.first, farthest.second, depth, link);

    int uDepthFromLca = depth[query.first] - depth[lowestCommon];

    if (uDepthFromLca >= query.second)
        return 1 + ancestor(query.first, query.second, link);

    return 1 + ancestor(farthest.second, farthest.first - query.second, link);
}

int main() {
    scanf("%d", &gladeNo);

    Tree tree(gladeNo);

    tree.parent[0] = -1;
    tree.sibling[0] = -1;

    for (int i = 0; i < gladeNo; i++) {
        int left, right;

        scanf("%d %d", &left, &right);

        tree.left[i] = left == -1 ? left : left - 1;

        tree.right[i] = right == -1 ? right : right - 1;

        if (left != -1) {
            tree.parent[left - 1] = i;
            tree.sibling[left - 1] = right == -1 ? right : right - 1;
        }

        if (right != -1) {
            tree.parent[right - 1] = i;
            tree.sibling[right - 1] = left == -1 ? left : left - 1;;
        }
    }

    int depth[gladeNo];

    tree.fillDepth(depth, 0, 0);

    height = floor(log2(gladeNo));
    pow2 = pow(2, height);

    int** link = new int *[height];
    for (int i = 0; i < height; i++)
        link[i] = new int[gladeNo];

    for (int v = 0; v < gladeNo; v++)
        link[0][v] = tree.parent[v];

    for (int i = 1; i < height; i++) {
        for (int v = 0; v < gladeNo; v++)
            link[i][v] = link[i - 1][v] == -1 ? -1 : link[i - 1][link[i - 1][v]];
    }

    pair<int, int> far[gladeNo];

    tree.fillFar(far);

    int queryNo;
    scanf("%d", &queryNo);

    for (int i = 0; i < queryNo; i++) {
        int u, dist;

        scanf("%d %d", &u, &dist);

        printf("%d\n", answerQuery(make_pair(u - 1, dist), depth, far, tree, link));
    }

    for(int i = 0; i < height; i++)
        delete[] link[i];

    delete[] link;

    return 0;
}