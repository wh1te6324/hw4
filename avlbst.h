#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    AVLNode<Key, Value>* getRoot() const;
    int height(AVLNode<Key, Value>* node) const;
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void rebalance(AVLNode<Key, Value>* node);
    int updateHeightAndBalance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if(this->root_ == NULL) {
        AVLNode<Key,Value>* newNode =
            new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        this->root_ = newNode;
        updateHeightAndBalance(getRoot());
        return;
    }

    Node<Key,Value>* cur = this->root_;
    Node<Key,Value>* parent = NULL;

    while(cur != NULL) {
        parent = cur;
        if(new_item.first < cur->getKey()) {
            cur = cur->getLeft();
        }
        else if(cur->getKey() < new_item.first) {
            cur = cur->getRight();
        }
        else {
            
            cur->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key,Value>* parentAVL = static_cast<AVLNode<Key,Value>*>(parent);
    AVLNode<Key,Value>* newNode =
        new AVLNode<Key,Value>(new_item.first, new_item.second, parentAVL);

    if(new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }
    rebalance(parentAVL);

    updateHeightAndBalance(getRoot());
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    Node<Key,Value>* node = this->internalFind(key);
    if(node == NULL) return;

    AVLNode<Key,Value>* z = static_cast<AVLNode<Key,Value>*>(node);

    
    if(z->getLeft() != NULL && z->getRight() != NULL) {
        Node<Key,Value>* predNode =
            BinarySearchTree<Key,Value>::predecessor(z);
        AVLNode<Key,Value>* pred =
            static_cast<AVLNode<Key,Value>*>(predNode);
        nodeSwap(z, pred);
    }

   
    AVLNode<Key,Value>* parent = z->getParent();
    AVLNode<Key,Value>* child;
    if(z->getLeft() != NULL) child = z->getLeft();
    else                     child = z->getRight();

    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        
        this->root_ = child;
    }
    else if(parent->getLeft() == z) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete z;

    
    if(parent != NULL) {
        rebalance(parent);
    }

    if(this->root_ != NULL) {
        updateHeightAndBalance(getRoot());
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::getRoot() const
{
    return static_cast<AVLNode<Key,Value>*>(this->root_);
}

template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const
{
    if(node == NULL) return 0;
    int hl = height(node->getLeft());
    int hr = height(node->getRight());
    return 1 + (hl > hr ? hl : hr);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getRight();
    if(y == NULL) return;

    AVLNode<Key,Value>* B = y->getLeft();
    AVLNode<Key,Value>* p = x->getParent();
    y->setLeft(x);
    x->setParent(y);
    x->setRight(B);
    if(B != NULL) B->setParent(x);

    y->setParent(p);

    if(p == NULL) {
        this->root_ = y;
    }
    else if(p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getLeft();
    if(y == NULL) return;

    AVLNode<Key,Value>* B = y->getRight();
    AVLNode<Key,Value>* p = x->getParent();

    
    y->setRight(x);
    x->setParent(y);
    x->setLeft(B);
    if(B != NULL) B->setParent(x);

    y->setParent(p);

    if(p == NULL) {
        this->root_ = y;
    }
    else if(p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    while(node != NULL) {
        int hl = height(node->getLeft());
        int hr = height(node->getRight());
        int bf = hl - hr;  

        if(bf > 1) { 
            AVLNode<Key,Value>* left = node->getLeft();
            int hll = height(left->getLeft());
            int hlr = height(left->getRight());

            if(hll >= hlr) {
                
                rotateRight(node);
            }
            else {
                
                rotateLeft(left);
                rotateRight(node);
            }
        }
        else if(bf < -1) { 
            AVLNode<Key,Value>* right = node->getRight();
            int hrl = height(right->getLeft());
            int hrr = height(right->getRight());

            if(hrr >= hrl) {
                
                rotateLeft(node);
            }
            else {
                
                rotateRight(right);
                rotateLeft(node);
            }
        }

        node = node->getParent();
    }
}
template<class Key, class Value>
int AVLTree<Key, Value>::updateHeightAndBalance(AVLNode<Key, Value>* node)
{
    if(node == NULL) return 0;

    int hl = updateHeightAndBalance(node->getLeft());
    int hr = updateHeightAndBalance(node->getRight());
    int bf = hl - hr;
    node->setBalance(static_cast<int8_t>(bf));

    return 1 + (hl > hr ? hl : hr);
}
#endif
