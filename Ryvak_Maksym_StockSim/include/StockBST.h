#ifndef STOCK_BST_H
#define STOCK_BST_H

#include <string>
#include <vector>
using namespace std;

// Binary Search Tree that indexes stocks by a numeric key (e.g. annual return %).
// Supports range queries: "find all stocks with return between X% and Y%"
// Also used in the Bonus: parameter sweep inserts results keyed by final portfolio value.
//
// Ordering invariant: left subtree keys < node key <= right subtree keys
//
// Course module: Trees (BST insert, search, traversals, height)
class StockBST {
public:
    struct BSTNode {
        string   ticker;
        double   key;    // the value the BST is sorted on (return %, final value, etc.)
        int      year;   // context field (which year this stat is for; 0 if not applicable)
        BSTNode* left;
        BSTNode* right;

        BSTNode(const string& ticker, double key, int year)
            : ticker(ticker), key(key), year(year), left(nullptr), right(nullptr) {}
    };

private:
    BSTNode* root;

    // Private recursive helpers — implement these, call from public interface
    BSTNode* insertHelper(BSTNode* node, const string& ticker, double key, int year);
    BSTNode* searchHelper(BSTNode* node, double key) const;
    void     rangeSearchHelper(BSTNode* node, double low, double high,
                               vector<BSTNode*>& results) const;
    void     inorderHelper(BSTNode* node)  const;
    void     preorderHelper(BSTNode* node) const;
    void     postorderHelper(BSTNode* node) const;
    int      heightHelper(BSTNode* node)   const;
    void     clearHelper(BSTNode* node);

public:
    StockBST();
    ~StockBST();

    // Insert a new entry. Duplicates allowed (go to right subtree).
    void insert(const string& ticker, double key, int year = 0);

    // Return pointer to a node whose key exactly matches. nullptr if not found.
    BSTNode* search(double key) const;

    // Collect all nodes where low <= key <= high into 'results' vector.
    void rangeSearch(double low, double high, vector<BSTNode*>& results) const;

    // Tree traversals — each prints: ticker (key) per line
    void inorder()   const;   // sorted ascending by key
    void preorder()  const;
    void postorder() const;

    // Returns the number of edges on the longest root-to-leaf path. -1 if empty.
    int getHeight() const;

    // Finds and returns the node with the maximum key (rightmost node).
    // Returns nullptr if tree is empty.
    BSTNode* findMax() const;

    // Frees all nodes. Resets root to nullptr.
    void clear();
};

#endif // STOCK_BST_H
