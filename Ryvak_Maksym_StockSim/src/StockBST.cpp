#include "StockBST.h"
#include <iostream>
#include <iomanip>

StockBST::StockBST() : root(nullptr) {}

StockBST::~StockBST() {
    clear();
}

StockBST::BSTNode* StockBST::insertHelper(BSTNode* node, const string& ticker, double key, int year) {
    if (!node) {
        return new BSTNode(ticker, key, year);
    }
    if (key < node->key) {
        node->left = insertHelper(node->left, ticker, key, year);
    } else {
        node->right = insertHelper(node->right, ticker, key, year);
    }
    return node;
}

StockBST::BSTNode* StockBST::searchHelper(BSTNode* node, double key) const {
    if (!node) {
        return nullptr;
    }
    if (node->key == key) {
        return node;
    }
    if (key < node->key) {
        return searchHelper(node->left, key);
    }
    return searchHelper(node->right, key);
}

void StockBST::rangeSearchHelper(BSTNode* node, double low, double high,
                                 vector<BSTNode*>& results) const {
    if (!node) {
        return;
    }
    if (low < node->key) {
        rangeSearchHelper(node->left, low, high, results);
    }
    if (node->key >= low && node->key <= high) {
        results.push_back(node);
    }
    if (high > node->key) {
        rangeSearchHelper(node->right, low, high, results);
    }
}

void StockBST::inorderHelper(BSTNode* node) const {
    if (!node) {
        return;
    }
    inorderHelper(node->left);
    cout << node->ticker << " (" << fixed << setprecision(2) << node->key << ")";
    if (node->year != 0) {
        cout << "  " << node->year;
    }
    cout << "\n";
    inorderHelper(node->right);
}

void StockBST::preorderHelper(BSTNode* node) const {
    if (!node) {
        return;
    }
    cout << node->ticker << " (" << fixed << setprecision(2) << node->key << ")";
    if (node->year != 0) {
        cout << "  " << node->year;
    }
    cout << "\n";
    preorderHelper(node->left);
    preorderHelper(node->right);
}

void StockBST::postorderHelper(BSTNode* node) const {
    if (!node) {
        return;
    }
    postorderHelper(node->left);
    postorderHelper(node->right);
    cout << node->ticker << " (" << fixed << setprecision(2) << node->key << ")";
    if (node->year != 0) {
        cout << "  " << node->year;
    }
    cout << "\n";
}

int StockBST::heightHelper(BSTNode* node) const {
    if (!node) {
        return -1;
    }
    int leftH = heightHelper(node->left);
    int rightH = heightHelper(node->right);
    return (leftH > rightH ? leftH : rightH) + 1;
}

void StockBST::clearHelper(BSTNode* node) {
    if (!node) {
        return;
    }
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void StockBST::insert(const string& ticker, double key, int year) {
    root = insertHelper(root, ticker, key, year);
}

StockBST::BSTNode* StockBST::search(double key) const {
    return searchHelper(root, key);
}

void StockBST::rangeSearch(double low, double high, vector<BSTNode*>& results) const {
    rangeSearchHelper(root, low, high, results);
}

void StockBST::inorder() const {
    inorderHelper(root);
}

void StockBST::preorder() const {
    preorderHelper(root);
}

void StockBST::postorder() const {
    postorderHelper(root);
}

int StockBST::getHeight() const {
    return heightHelper(root);
}

StockBST::BSTNode* StockBST::findMax() const {
    if (!root) {
        return nullptr;
    }
    BSTNode* current = root;
    while (current->right) {
        current = current->right;
    }
    return current;
}

void StockBST::clear() {
    clearHelper(root);
    root = nullptr;
}
