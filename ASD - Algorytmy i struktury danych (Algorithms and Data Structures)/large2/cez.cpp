#include <iostream>
#include <cassert>

using namespace std;

typedef struct Node {
    Node* left;
    Node* right;
    Node* parent;
    bool gaul, swap;
    unsigned long long size, GG, GR, RG, RR;
} Node;

const unsigned long long BILLION = 1000000007;

class SplayTree {
public:
    Node* root;
    string word;

    SplayTree(const string& word, int first, int last);

    void resetCounts(Node* v);
    Node* createNode(bool gaul, Node* parent);
    void updateSequenceCountsFromLeft(Node*left, Node* father);
    void updateSequenceCountsFromRight(Node* right, Node* father);
    Node* createSplay(int first, int last, Node* parent);
    void maybeSwap(Node* v);
    void fullRecount(Node* lower, Node* higher);
    void resetSize(Node* v);
    void rotateRight(Node* prevHigher);
    void rotateLeft(Node* prevHigher);
    void splay(Node* moved);
    Node* getMin(Node* rt);
    Node* getMax(Node* rt);
    Node* findKeyHelp(Node* node, int key, int parentKey, bool isRight);
    Node* findKey(Node* rt, int key);
    Node* join(Node* left, Node* right);
    void split(Node*& left, Node*& right);
    void prepareTreeParts(int i, int j, Node*& left, Node*& mid, Node*& right);
    void reconnectTreeParts(int i, int j, Node*& left, Node*& mid, Node*& right);
    int howManyPatrols(int i, int j);
    void reverseOrder(int i, int j);
    void deleteTree(Node* node);
};

SplayTree::SplayTree(const string& word, int first, int last) {
    this->word = word;

    this->root = createSplay(first, last, nullptr);
}

void SplayTree::resetCounts(Node *v) {
    v->GR = 0;
    v->RG = 0;

    if (v->gaul) {
        v->GG = 1;
        v->RR = 0;
    }
    else {
        v->RR = 1;
        v->GG = 0;
    }
}

Node *SplayTree::createNode(bool gaul, Node *parent) {
    Node* ret = new Node();

    ret->gaul = gaul;
    ret->left = nullptr;
    ret->right = nullptr;
    ret->parent = parent;
    ret->swap = false;
    ret->size = 1;

    resetCounts(ret);

    return ret;
}

void SplayTree::updateSequenceCountsFromLeft(Node *left, Node *father) {
    unsigned long long tmp[4];

    tmp[0] = (left->GG * father->RG + left->GR * father->GG + left->GR * father->RG + father->GG + left->GG) % BILLION;
    tmp[1] = (left->GG * father->RR + left->GR * father->RR + left->GR * father->GR + father->GR + left->GR) % BILLION;
    tmp[2] = (left->RG * father->RG + left->RR * father->RG + left->RR * father->GG + father->RG + left->RG) % BILLION;
    tmp[3] = (left->RR * father->RR + left->RR * father->GR + left->RG * father->RR + father->RR + left->RR) % BILLION;

    father->GG = tmp[0];
    father->GR = tmp[1];
    father->RG = tmp[2];
    father->RR = tmp[3];
}

void SplayTree::updateSequenceCountsFromRight(Node *right, Node *father) {
    unsigned long long tmp[4];

    tmp[0] = (father->GG * right->RG + father->GR * right->GG + father->GR * right->RG + right->GG + father->GG) % BILLION;
    tmp[1] = (father->GG * right->RR + father->GR * right->RR + father->GR * right->GR + right->GR + father->GR) % BILLION;
    tmp[2] = (father->RG * right->RG + father->RR * right->RG + father->RR * right->GG + right->RG + father->RG) % BILLION;
    tmp[3] = (father->RR * right->RR + father->RR * right->GR + father->RG * right->RR + right->RR + father->RR) % BILLION;

    father->GG = tmp[0];
    father->GR = tmp[1];
    father->RG = tmp[2];
    father->RR = tmp[3];
}

Node *SplayTree::createSplay(int first, int last, Node *parent) {
    if (first > last)
        return nullptr;

    int m = (first + last) / 2;
    Node* newNode = createNode(word[m] == 'G', parent);

    newNode->left = createSplay(first, m - 1, newNode);
    newNode->right = createSplay(m + 1, last, newNode);

    if (newNode->left) {
        newNode->size += newNode->left->size;

        updateSequenceCountsFromLeft(newNode->left, newNode);
    }

    if (newNode->right) {
        newNode->size += newNode->right->size;

        updateSequenceCountsFromRight(newNode->right, newNode);
    }

    return newNode;
}

void SplayTree::maybeSwap(Node *v) {
    if (v && v->swap) {
        v->swap = false;

        Node *tmp = v->left;
        v->left = v->right;
        v->right = tmp;

        // Wystarczy zmienić te, bo odwrócenie kolejności to takie odbicie lustrzane,
        // więc te, co były GR, będą teraz RG i na odwrót, a GG i RR zostaną właściwie
        // te same (nie do końca, ale kolejność w nich nie ma znaczenia)
        unsigned long long tmpGR = v->GR;
        v->GR = v->RG;
        v->RG = tmpGR;

        if (v->left)
            v->left->swap = !(v->left->swap);

        if (v->right)
            v->right->swap = !(v->right->swap);
    }
}

void SplayTree::fullRecount(Node *lower, Node *higher) {
    resetCounts(lower);
    resetCounts(higher);

    // Najpierw przeliczamy niższy, bo jego wartość wpływa na wyższy
    if (lower->left)
        updateSequenceCountsFromLeft(lower->left, lower);

    if (lower->right)
        updateSequenceCountsFromRight(lower->right, lower);

    if (higher->left)
        updateSequenceCountsFromLeft(higher->left, higher);

    if (higher->right)
        updateSequenceCountsFromRight(higher->right, higher);
}

void SplayTree::resetSize(Node *v) {
    v->size = 1;

    if (v->right)
        v->size += v->right->size;
    if (v->left)
        v->size += v->left->size;
}

void SplayTree::rotateRight(Node* prevHigher) {
    Node* parent = prevHigher->parent;
    Node* newHigher = prevHigher->left;

    prevHigher->left = newHigher->right;

    if (prevHigher->left) {
        maybeSwap(prevHigher->left);
        prevHigher->left->parent = prevHigher;
    }

    // Dolny węzeł wskakuje na miejsce górnego, jako odpowiedni syn ojca
    if (parent) {
        if (prevHigher == parent->left)
            parent->left = newHigher;
        else
            parent->right = newHigher;
    }

    newHigher->parent = parent;
    newHigher->right = prevHigher;
    prevHigher->parent = newHigher;

    // Zmieniają się poddrzewa obu węzłów, czyli reprezentują
    // nowy odcinek - trzeba policzyć od nowa.
    resetSize(prevHigher);
    resetSize(newHigher);

    fullRecount(prevHigher, newHigher);
}

void SplayTree::rotateLeft(Node *prevHigher) {
    Node* parent = prevHigher->parent;
    Node* newHigher = prevHigher->right;

    prevHigher->right = newHigher->left;

    if (prevHigher->right) {
        maybeSwap(prevHigher->right);
        prevHigher->right->parent = prevHigher;
    }

    // Dolny węzeł wskakuje na miejsce górnego, jako odpowiedni syn ojca
    if (parent) {
        if (prevHigher == parent->left)
            parent->left = newHigher;
        else
            parent->right = newHigher;
    }

    newHigher->parent = parent;
    newHigher->left = prevHigher;
    prevHigher->parent = newHigher;

    // Zmieniają się poddrzewa obu węzłów, czyli reprezentują
    // nowy odcinek - trzeba policzyć od nowa.
    resetSize(prevHigher);
    resetSize(newHigher);

    fullRecount(prevHigher, newHigher);
}

void SplayTree::splay(Node *moved) {
    while (moved->parent != nullptr) {
        if (moved->parent->parent == nullptr) {
            maybeSwap(moved->parent);
            maybeSwap(moved);

            if (moved == moved->parent->left) {
                maybeSwap(moved->parent->right);
                rotateRight(moved->parent);
            }
            else {
                maybeSwap(moved->parent->left);
                rotateLeft(moved->parent);
            }
        }
        else {
            maybeSwap(moved->parent->parent);
            maybeSwap(moved->parent);
            maybeSwap(moved);

            if (moved == moved->parent->left && moved->parent == moved->parent->parent->left) {
                // Wołamy maybeSwap na tych, które są "w zasięgu", ale nie są moved->parent, ani moved.
                maybeSwap(moved->parent->parent->right);
                maybeSwap(moved->parent->right);

                rotateRight(moved->parent->parent);
                rotateRight(moved->parent);
            }
            else if (moved == moved->parent->right && moved->parent == moved->parent->parent->right) {
                maybeSwap(moved->parent->parent->left);
                maybeSwap(moved->parent->left);

                rotateLeft(moved->parent->parent);
                rotateLeft(moved->parent);
            }
            else if (moved == moved->parent->right && moved->parent == moved->parent->parent->left) {
                maybeSwap(moved->parent->parent->right);
                maybeSwap(moved->parent->left);

                rotateLeft(moved->parent);
                rotateRight(moved->parent);
            }
            else { // moved == moved->parent->left && moved->parent == moved->parent->parent->right
                maybeSwap(moved->parent->parent->left);
                maybeSwap(moved->parent->right);

                rotateRight(moved->parent);
                rotateLeft(moved->parent);
            }
        }
    }
}

// Zasada BST zachowana - najmniejszy najbardziej na lewo
Node *SplayTree::getMin(Node *rt) {
    Node* prev = rt;

    while (rt != nullptr) {
        maybeSwap(rt);

        prev = rt;
        rt = rt->left;
    }

    return prev;
}

// Zasada BST zachowana - największy najbardziej na prawo
Node *SplayTree::getMax(Node *rt) {
    Node* prev = rt;

    while (rt != nullptr) {
        maybeSwap(rt);

        prev = rt;
        rt = rt->right;
    }

    return prev;
}

// Liczymy klucze implicite, ale poruszamy się na zasadach BST
Node *SplayTree::findKeyHelp(Node *node, int key, int parentKey, bool isRight) {
    if (node == nullptr)
        return node;

    maybeSwap(node);

    int leftSize, rightSize, myKey;

    if (node->left) {
        maybeSwap(node->left);
        leftSize = node->left->size;
    }
    else {
        leftSize = 0;
    }

    if (node->right) {
        maybeSwap(node->right);
        rightSize = node->right->size;
    }
    else {
        rightSize = 0;
    }

    if (isRight)
        myKey = leftSize + parentKey + 1;
    else
        myKey = parentKey - rightSize - 1;

    if (myKey == key)
        return node;

    if (myKey > key)
        return findKeyHelp(node->left, key, myKey, false);

    // myKey < key
    return findKeyHelp(node->right, key, myKey, true);
}

Node *SplayTree::findKey(Node *rt, int key) {
    Node* ret;

    if (key == 1)
        ret = getMin(rt);
    else if (key == word.length())
        ret = getMax(rt);
    else
        ret = findKeyHelp(rt, key, 0, true);

    splay(ret);

    return ret;
}

Node *SplayTree::join(Node *left, Node *right) {
    if (left == nullptr)
        return right;

    if (right == nullptr)
        return left;

    Node *tmpRoot = getMax(left);
    splay(tmpRoot);

    // Splay na największym wrzuci go na szczyt bez prawego syna
    // assert(tmpRoot->right == nullptr);

    tmpRoot->right = right;
    tmpRoot->size += right->size;
    right->parent = tmpRoot;

    resetCounts(tmpRoot);
    if (tmpRoot->left != nullptr)
        updateSequenceCountsFromLeft(tmpRoot->left, tmpRoot);

    if (tmpRoot->right != nullptr)
        updateSequenceCountsFromRight(tmpRoot->right, tmpRoot);

    return tmpRoot;
}

// Węzeł, w którym rozdzielamy, to jednocześnie korzeń nowego lewego drzewa
void SplayTree::split(Node *&left, Node *&right) {
    splay(left);

    if (left->right != nullptr) {
        right = left->right;
        right->parent = nullptr;

        left->size -= right->size;
    }
    else {
        right = nullptr;
    }

    left->right = nullptr;

    // Dodatkowa zmiana w poddrzewach, poza tymi w splayu, wynika tylko
    // z odłączenia right od left. W prawym bez zmian.
    resetCounts(left);
    if (left->left != nullptr)
        updateSequenceCountsFromLeft(left->left, left);
}

void SplayTree::prepareTreeParts(int i, int j, Node *&left, Node *&mid, Node *&right) {
    int leftSize;

    if (i > 1) {
        left = findKey(root, i - 1);
        split(left, mid);
        leftSize = left->size;
    }
    else {
        // Żeby i tak zrobić splaya, który po drodze propaguje swapy
        root = findKey(root, i);
        mid = root;
        leftSize = 0;
    }

    // Odejmujemy leftSize, bo w wyekstrahowanym środkowym poddrzewie liczymy klucze od nowa
    mid = findKey(mid, j - leftSize);

    if (j < word.length())
        split(mid, right);
}

void SplayTree::reconnectTreeParts(int i, int j, Node *&left, Node *&mid, Node *&right) {
    // Tylko, jeśli wcześniej odłączaliśmy.
    if (i > 1)
        mid = join(left, mid);

    if (j < word.length())
        mid = join(mid, right);

    root = mid;
}

int SplayTree::howManyPatrols(int i, int j) {
    Node *left, *mid, *right;
    unsigned long long ret;

    prepareTreeParts(i, j, left, mid, right);

    ret = (mid->GG + mid->GR + mid->RG + mid->RR) % BILLION;

    reconnectTreeParts(i, j, left, mid, right);

    return ret;
}

void SplayTree::reverseOrder(int i, int j) {
    Node *left, *mid, *right;

    prepareTreeParts(i, j, left, mid, right);

    mid->swap = true;
    maybeSwap(mid);

    reconnectTreeParts(i, j, left, mid, right);
}

void SplayTree::deleteTree(Node *node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);

        delete node;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int soldierNum, queryNum;
    string word;

    cin >> soldierNum;
    cin >> queryNum;
    cin >> word;

    SplayTree tree(word, 0, soldierNum - 1);

    for (int k = 0; k < queryNum; k++) {
        int i, j;
        char queryType;

        cin >> queryType;
        cin >> i;
        cin >> j;

        if (queryType == '?')
            printf("%d\n", tree.howManyPatrols(i, j));
        else
            tree.reverseOrder(i, j);
    }

    tree.deleteTree(tree.root);

    return 0;
}