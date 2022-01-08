#ifndef WET2_RANKEDAVL_H
#define WET2_RANKEDAVL_H

#include <iostream>

using std::cout;

class rankedavl;
class RankedNode;
typedef void (*destroy_func)(void*);

// Needs to take the tree as an argument because this is a static function.
typedef RankedNode* (*find_func)(rankedavl*, void*);

// cmp_func(a,b) < 0 iff "a < b" according to the strict order imposed by cmp_func
// and similarly cmp_func(a,b) == 0 iff "a ~ b".
typedef int (*cmp_func)(void*, void*);

// The basic node used in trees and lists, with added rank.
class RankedNode
{
    void* content;
    int rank;
    RankedNode* father;
    RankedNode* left;
    RankedNode* right;
    int height;
    friend class rankedavl;
    friend int height(const RankedNode* const node);
    friend RankedNode* findNext(const RankedNode* const node);
    friend void deleteTree(RankedNode* node, destroy_func destroyFunc);
	friend int leftRank(const RankedNode* const node);
	friend int rightRank(const RankedNode* const node);

public:
    RankedNode() : content(nullptr), rank(1), father(nullptr),
        left(nullptr), right(nullptr), height(0) { }

    int getHeight() const { return this->height; }
    void* getContent() { return this->content; }
    RankedNode& operator=(const RankedNode& node) {
        this->father = node.father;
        this->height = node.height;
        this->left = node.left;
        this->right = node.right;
        this->content = node.content;
        return *this;
    }

    int getBF() const;
    static void switchNodes(RankedNode** a, RankedNode** b);
};

// The AVL tree, with ranks this time.
class rankedavl
{
    RankedNode* head;
    find_func findFunc;
    destroy_func destroyFunc;
    cmp_func compareFunc;

public:
    // If find == nullptr, assigns it the default genericFind function.
    rankedavl(find_func find = nullptr, destroy_func destroy = nullptr, cmp_func cmp = nullptr)
        : head(nullptr), findFunc(find), destroyFunc(destroy), compareFunc(cmp) { }
    ~rankedavl();

    void setHead(RankedNode* node) { this->head = node; }
    RankedNode* getHead() const { return head; }
    rankedavl& operator=(const rankedavl& node)
    {
        this->head = node.head;
        return *this;
    }
    RankedNode* find(void* content) { return this->findFunc(this, content); }

    RankedNode* insert(void* content);
    void remove(destroy_func destroyFunc, void* content);
    void remove_aux(destroy_func destroyFunc, RankedNode* node);
    bool rotate(RankedNode** Z, int id = -1);
    RankedNode* leftRotation(RankedNode** Z);
    RankedNode* rightRotation(RankedNode** Z);
    RankedNode* RLRotation(RankedNode** Z);
    RankedNode* RRRotation(RankedNode** Z);
    RankedNode* LRRotation(RankedNode** Z);
    RankedNode* LLRotation(RankedNode** Z);
    RankedNode* findSuccessor(RankedNode* curr, cmp_func compareFunc);
	
	// content field only needs "key" fields filled in!
	bool indexOf(void* content, int* index);
	bool select(int index, void** content);

    // A generic find algorithm that relies on compareFunc to compare nodes in the tree.
    static RankedNode* genericFind(rankedavl* tree, void* content)
    {
        if (tree->getHead() == nullptr) return nullptr;
        RankedNode* curr = tree->head;
        RankedNode* prev = tree->head;

        while (curr != nullptr)
        {
            int result = tree->compareFunc(curr->getContent(), content);
            if (result == 0) return curr;
            else {
                prev = curr;
                if (result > 0) curr = curr->left;
                else curr = curr->right;
            }
        }

        return prev;
    }
};

#endif // WET2_RANKEDAVL_H
