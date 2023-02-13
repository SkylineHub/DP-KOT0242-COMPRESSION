#include <iostream>
#pragma once

using namespace std;

struct Node
{
    char key{};
    u_long freq{};
    Node* parent;
    Node** child{};
    bool isLast{};
    u_long lower{};
    u_long upper{};
    u_long currentLow{};
};

Node *newNode(char key)
{
    Node *node = new Node;
    node->key = key;
    node->child = (Node**) calloc(128, sizeof(Node*));
    return node;
}

Node* newNode(char key, Node* parent, bool isLast)
{
    Node *node = new Node;
    node->key = key;
    node->parent = parent;
    node->isLast = isLast;
    node->child = (Node**) calloc(128, sizeof(Node*));
    return node;
}

Node* newNode(char key, Node* parent, u_long freq)
{
    Node *node = new Node;
    node->key = key;
    node->parent = parent;
    node->freq = freq;
    node->child = (Node**) calloc(128, sizeof(Node*));
    return node;
}

Node* addChild(Node *root, char x, bool isLast) {
    if (root->child == nullptr) {
        root->child = (Node**) calloc(128, sizeof(Node*));
    }
    if (root->child[x] == nullptr) {
        root->child[x] = newNode(x, root, u_long(1));
    } else {
        root->child[x]->freq += 1;
    }
    return root->child[x];
}

void *addChildTest(Node *root, char x, bool isLast) {
    if (&root->child[x] == nullptr) {
        root->child[x] = newNode(x, root, isLast);
    } else {
        root->child[x]->freq += 1;
    }
}

void addNode(Node *root, char a) {
    addChild(root, a, true);
}

void addNode(Node *root, char a, char b) {
    Node* i = addChild(root, a, false);
    addChild(i, b, true);
    //cout << "Last 1" << endl;
}

void addNode(Node *root, char a, char b, char c) {
    Node* i = addChild(root, a, false);
    Node* j = addChild(i, b, false);
    addChild(j, c, true);
    //cout << "Last 2" << endl;
}

void addNode(Node *root, char a, char b, char c, char d) {
    Node* i = addChild(root, a, false);
    Node* j = addChild(i, b, false);
    Node* l = addChild(j, c, false);
    addChild(l, d, true);
    //cout << "Last 3" << endl;
}

void addNode(Node *root, char a, char b, char c, char d, char e) {
    Node* child = addChild(root, a, false);
    child = addChild(child, b, false);
    child = addChild(child, c, false);
    child = addChild(child, d, false);
    addChild(child, e, true);
}

void addNode(Node *root, char a, char b, char c, char d, char e, char f) {
    Node* child = addChild(root, a, false);
    child = addChild(child, b, false);
    child = addChild(child, c, false);
    child = addChild(child, d, false);
    child = addChild(child, e, false);
    addChild(child, f, false);
}

void addNodeTest(Node *root, char a, char b, char c, char d, char e, char f) {
    addChildTest(root, a, false);
    addChildTest(root->child[a], b, false);
    addChildTest(root->child[a]->child[b], c, false);
    addChildTest(root->child[a]->child[b]->child[c], d, false);
    addChildTest(root->child[a]->child[b]->child[c]->child[d], e, false);
    addChildTest(root->child[a]->child[b]->child[c]->child[d]->child[e], f, false);
}

void addLastNode(Node *root, char a, char b, char c, char d, char e, char f) {
    Node* child = addChild(root, a, false);
    child = addChild(child, b, false);
    child = addChild(child, c, false);
    child = addChild(child, d, false);
    child = addChild(child, e, false);
    addChild(child, e, true);
    //cout << "Last 4" << endl;
}
