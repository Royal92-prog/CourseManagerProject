#include "rankedavl.h"

#include <iostream>

int max(int a, int b) { return a > b ? a : b; }

int getHeight(RankedNode* n) { return (n != nullptr) ? n->getHeight() : -1; }

// Calculates the height of a given node within the AVL tree.
int height(const RankedNode* const node)
{
    if (node == nullptr) return -1;
    int L = height(node->left),
        R = height(node->right);
    return 1 + max(L, R);
}

// Returns the Balance Factor of the node.
int RankedNode::getBF() const
{
    // Have to call through the global namespace, otherwise
    // assumes I'm talking about RankedNode::getHeight for some reason
    return ::getHeight(this->left) - ::getHeight(this->right);
}

int leftRank(const RankedNode* const node)
{
	if (node->left == nullptr) return 0;
	return node->left->rank;
}

int rightRank(const RankedNode* const node)
{
	if (node->right == nullptr) return 0;
	return node->right->rank;
}

RankedNode* rankedavl::rightRotation(RankedNode** Z)
{
    RankedNode* Y = (*Z)->left;
    RankedNode* X = Y->right;
    Y->right = (*Z);
    (*Z)->left = X;
    Y->father = (*Z)->father;
    (*Z)->father = Y;
    if (X != nullptr) X->father = *Z;

    (*Z)->height = max(getHeight((*Z)->left), getHeight((*Z)->right)) + 1;
	(*Z)->rank = leftRank(*Z) + rightRank(*Z) + 1;
    Y->height = max(getHeight((*Y).left), getHeight((*Y).right)) + 1;
	Y->rank = leftRank(Y) + rightRank(Y) + 1;
    return Y;
}

RankedNode* rankedavl::leftRotation(RankedNode** Z) // Completely symmetrical to rightRotation...
{
    RankedNode* Y = (*Z)->right;
    RankedNode* X = Y->left;
    Y->left = (*Z);
    (*Z)->right = X;
    Y->father = (*Z)->father;
    (*Z)->father = Y;
    if (X != nullptr) X->father = *Z;

    (*Z)->height = max(getHeight((*Z)->left), getHeight((*Z)->right)) + 1;
	(*Z)->rank = leftRank(*Z) + rightRank(*Z) + 1;
    Y->height = max(getHeight(Y->left), getHeight(Y->right)) + 1;
	Y->rank = leftRank(Y) + rightRank(Y) + 1;
    return Y;
}

RankedNode* rankedavl::LLRotation(RankedNode** Z)
{
    return this->rightRotation(Z);
}

RankedNode* rankedavl::LRRotation(RankedNode** Z)
{
    (*Z)->left = this->leftRotation(&(*Z)->left);
    return this->rightRotation(Z);
}

RankedNode* rankedavl::RRRotation(RankedNode** Z) // Symmetrical to LLRotation.
{
    return this->leftRotation(Z);
}

RankedNode* rankedavl::RLRotation(RankedNode** Z) // Symmetrical to LRRotation.
{
    (*Z)->right = this->rightRotation(&(*Z)->right);
    return this->leftRotation(Z);
}

// Determines if any rotations are needed on Z, and performs them.
// Also returns whether or not any rotations actually happened.
bool rankedavl::rotate(RankedNode** Z, int id)
{
    RankedNode* X = (*Z)->father;
    RankedNode* root = nullptr;
    if ((*Z)->getBF() == 2) // Need LL or LR rotation. Check further.
    {
        if ((*Z)->left && (*Z)->left->getBF() >= 0) root = LLRotation(Z);
        else root = LRRotation(Z);

        if (X == nullptr) setHead(root);
        else if (this->compareFunc(X->content, root->content) < 0) X->right = root;
        else X->left = root;
        return true;
    }

    else if ((*Z)->getBF() == -2) // Need RR or RL rotation. Check further. Symmetrical to above.
    {
        if ((*Z)->right && (*Z)->right->getBF() <= 0) root = RRRotation(Z);
        else root = RLRotation(Z);

        if (X == nullptr) setHead(root);
        else if (this->compareFunc(X->content, root->content) < 0) X->right = root;
        else X->left = root;
        return true;
    }

    return false; // No rotations needed.
}

// Swaps between two nodes in the AVL tree.
void RankedNode::switchNodes(RankedNode** a, RankedNode** b)
{
    void* temp2 = (*a)->content;
    (*a)->content = (*b)->content;
    (*b)->content = temp2;
}

// Finds the next node within the subtree of the given node.
RankedNode* findNext(const RankedNode* const node)
{
    if (node->right == nullptr) return nullptr;
    RankedNode* current = node->right;
    while (current->left != nullptr) current = current->left;
    return current;
}

// Finds the next node in the whole tree.
RankedNode* rankedavl::findSuccessor(RankedNode* curr, cmp_func compareFunc)
{
    if (this->head == curr && curr->right == nullptr) return nullptr;
    if (curr->right != nullptr) return findNext(curr);
    if (curr->father != nullptr && compareFunc(curr->father->content, curr->content) < 0)
    {
        RankedNode* successor = curr->father;
        while (successor->father != nullptr)
        {
            if (compareFunc(successor->content, successor->father->content) < 0) return successor->father;
            successor = successor->father;
        }
        return nullptr;
    }
    return curr->father;
}

RankedNode* rankedavl::insert(void* content)
{
    RankedNode* node = nullptr;
    try { node = new RankedNode; }
    catch (std::bad_alloc&) { return nullptr; }
    node->content = content;
    if (this->getHead() == nullptr)
    {
        this->head = node;
        return this->getHead();
    }

    // We don't want the node to already exist in the tree...
    RankedNode* Z = this->find(content);
    if (this->compareFunc(node->content, Z->content) == 0)
    {
        delete node;
        return nullptr;
    }

    // Otherwise place it correctly in the tree...
    else if (this->compareFunc(node->content, Z->content) < 0) Z->left = node;
    else Z->right = node;
    node->father = Z;
    RankedNode* son = node;
    while (Z != nullptr)  // can't break this time, must recalculate rank for whole search path.
    {
        Z->height = son->height + 1;
        this->rotate(&Z);
		Z->rank = leftRank(Z) + rightRank(Z) + 1; // recalculate rank AFTER rotating, if needed
        son = Z;
        Z = Z->father;
    }

    return node;
}

// If node has one son at most, deletes it from the tree it's in.
void rankedavl::remove_aux(destroy_func destroyFunc, RankedNode* node)
{
    if (node == nullptr) return; // Unless nobody's home...

    RankedNode* father = node->father;
    if (node->left == nullptr && node->right == nullptr)
    {
        if (father->left != nullptr && this->compareFunc(father->left->content, node->content) == 0)
            father->left = nullptr;

        else if (father->right != nullptr && this->compareFunc(father->right->content, node->content) == 0)
            father->right = nullptr;

        if (destroyFunc != nullptr) destroyFunc(node->content);
        delete node;
    }
    else if (node->left != nullptr && node->right == nullptr)
    {
        RankedNode::switchNodes(&node, &node->left);
        if (destroyFunc != nullptr) destroyFunc(node->left->content);
        delete node->left;
        node->left = nullptr;
        node->height = height(node);
		node->rank = leftRank(node) + rightRank(node) + 1;
    }
    else if (node->left == nullptr && node->right != nullptr)
    {
        RankedNode::switchNodes(&node, &node->right);
        if (destroyFunc != nullptr) destroyFunc(node->right->content);
        delete node->right;
        node->right = nullptr;
        node->height = height(node);
		node->rank = leftRank(node) + rightRank(node) + 1;
    }
}

// Removes the node with the given content from the tree.
void rankedavl::remove(destroy_func destroyFunc, void* content)
{
    if (this->getHead() == nullptr) return; // Unless nobody's home...
    RankedNode* A = this->find(content);
    if (A == nullptr || this->compareFunc(A->content, content) != 0) return;
    
    RankedNode* father = A->father;
    if (A == this->head && A->left == nullptr && A->right == nullptr)
    {
        if (destroyFunc != nullptr) destroyFunc(A->content);
        delete A;
        this->head = nullptr;
    }
    else {
        if (A->left != nullptr && A->right != nullptr)
        {
            RankedNode* next = findNext(A);
            father = next->father;
            RankedNode::switchNodes(&A, &next);
            remove_aux(destroyFunc, next);
        }
        else remove_aux(destroyFunc, A);

        RankedNode* v = father;
        while (v != nullptr) // Need to rebalance the tree along the way...
        {
            v->height = max(getHeight(v->left), getHeight(v->right)) + 1;
            this->rotate(&v);
            v->rank = leftRank(v) + rightRank(v) + 1;
            v = v->father;
        }
    }
}

bool rankedavl::indexOf(void* content, int* index)
{
    if (index == nullptr) return false;
    RankedNode* node = this->find(content);
    if (node == nullptr) return false;

    *index = node->rank + leftRank(node);

    while (node->father != nullptr)
	{
        if (node == node->father->right)
            *index = *index + 1 + leftRank(node->father);

        node = node->father;
    }
    return true;
}

bool rankedavl::select(int index, void** content)
{
    if (content == nullptr) return false;
    RankedNode* current = head;
    int currentIndex = 0, nextIndex = 0;
    do
	{
        nextIndex = currentIndex + 1 + leftRank(current);
        if (nextIndex == index)
		{
            *content = current->content;
            return true;
        }
        if (nextIndex < index)
		{
            currentIndex = nextIndex;
            current = current->right;
        }
        else current = current->left;
    }
	while (current != nullptr);

    return false; // index not found in tree!
}

// Destroys node's subtree.
void deleteTree(RankedNode* node, destroy_func destroyFunc)
{
    if (node == nullptr) return;
    deleteTree(node->left, destroyFunc);
    if (destroyFunc != nullptr && node->getContent() != nullptr) destroyFunc(node->getContent());
    deleteTree(node->right, destroyFunc);
    delete node;
}

rankedavl::~rankedavl()
{
    deleteTree(this->head, this->destroyFunc);
    this->head = nullptr;
}
