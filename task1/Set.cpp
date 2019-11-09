#ifndef __SET_H__
#define __SET_H__
#include <cstring>
#include <cassert>
typedef size_t size_type;

template<class value_type>
class Set {
private:
    const bool RED=1;
    const bool BLACK=0;
    size_type number;
    class Node {
    public:
        value_type key;
        Node *left, *right, *leftList, *rightList;
        bool color;
        Node(value_type Key, bool Color){
            key = Key;
            color = Color;
            left = nullptr;
            right = nullptr;
            leftList = this;
            rightList = this;
        }
    };
    
    Node * root;
    Node * rotateLeft(Node *h);
    Node * rotateRight(Node *h);
    Node * colorFlip(Node *h);
    Node * insert(Node *h, Node *parent, int direction, value_type key);
    Node * insertToLeft(Node *h, Node *parent);
    Node * insertToRight(Node *h, Node *parent);
    Node * deleteList(Node *h);
    Node * fixUp(Node *h);
    Node * moveRedRight(Node *h);
    Node * moveRedLeft(Node *h);
    value_type getMin(Node *h);
    Node * deleteIn(Node *h, value_type key);
    Node * deleteMin(Node *h);
    size_type ifFind(value_type key);
    bool isRed(Node *x);
public:
    class iterator {
    public:
        Node *toNode;
        iterator() {
            toNode = nullptr;
        }
        iterator(Node *node) {
            toNode = node;
        }
        value_type & operator *() {
            return toNode->key;
        }
        void operator++(int) {
            toNode = toNode->rightList;
        }
        void operator--(int) {
            toNode = toNode->leftList;
        }
        iterator & operator=(const iterator & b) {
            toNode = b.toNode;
            return *this;
        }
    };

    void dedddbug(Node *h);
    Set() {
        number = 0;
        root = nullptr;
    }
    bool empty() const;
    size_type size() const;
    void clear();
    void insert(const value_type& value);
    size_type erase(const value_type& value);
    size_type count(const value_type& value);
    iterator find(const value_type& value);
    iterator lower_bound(const value_type& value);
    iterator upper_bound(const value_type& value);
};

template<class value_type>
bool Set<value_type>::isRed(Node *x) {
    if(x==nullptr)
        return false;
    return x->color==RED;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::rotateLeft(Node *h) {
    Node *x=h->right;
    h->right = x->left;
    x->left = h;
    x->color = x->left->color;
    x->left->color = RED;

    return x;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::rotateRight(Node *h) {
    Node *x=h->left;
    h->left = x->right;
    x->right = h;
    x->color = x->right->color;
    x->right->color = RED;
    return x;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::colorFlip(Node *h) {
    
    h->color = !h->color;
    if(h->left!=nullptr)h->left->color = !h->left->color;
    if(h->right!=nullptr)h->right->color = !h->right->color;
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::fixUp(Node *h) {
    if(isRed(h->right))
        h = rotateLeft(h);
    if(isRed(h->left) && isRed(h->left->left))
        h = rotateRight(h);
    if(isRed(h->left) && isRed(h->right))
        h = colorFlip(h);
    
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::insertToLeft(Node *h, Node *parent) {
    parent->leftList->rightList = h;
    h->leftList = parent->leftList;
    parent->leftList = h;
    h->rightList = parent;
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::insertToRight(Node *h, Node *parent) {
    parent->rightList->leftList = h;
    h->rightList = parent->rightList;
    parent->rightList = h;
    h->leftList = parent;
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::insert(Node *h, Node *parent, int direction, value_type key) {
    if(h==nullptr) {
        number++;
        Node *x=new Node(key, RED);
        if(parent!=nullptr) {
            if(direction==0)    x = insertToLeft(x, parent);
            else    x = insertToRight(x, parent);
        }
        return x;
    }
    if(key==h->key)  ;
    else if(key<h->key)
        h->left = insert(h->left, h, 0, key);
    else
        h->right = insert(h->right, h, 1, key);
    if(isRed(h->right) && !isRed(h->left))
        h = rotateLeft(h);
    if(h->left!=nullptr && isRed(h->left) && isRed(h->left->left))
        h = rotateRight(h);
    if(isRed(h->left) && isRed(h->right))
        h = colorFlip(h);
    
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::moveRedRight(Node *h) {
    h = colorFlip(h);
    if(h->left!=nullptr && isRed(h->left->left)) {
        h = rotateRight(h);
        h = colorFlip(h);
    }
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::moveRedLeft(Node *h) {
    h = colorFlip(h);
    if(h->right!=nullptr && isRed(h->right->left)) {
        h->right = rotateRight(h->right);
        h = rotateLeft(h);
        colorFlip(h);
    }
    return h;
}

template<class value_type>
value_type Set<value_type>::getMin(Node *h) {
    Node *x=h;
    while(x->left!=nullptr) x = x->left;
    return x->key;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::deleteList(Node *h) {
    h->rightList->leftList = h->leftList;
    h->leftList->rightList = h->rightList;
    h->leftList = h;
    h->rightList = h;
    return h;
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::deleteMin(Node *h) {
    if(h==nullptr)  return nullptr;
    if(h->left==nullptr) {
        h = deleteList(h);
        delete h;
        number--;
        return nullptr;
    }
    if(!isRed(h->left) && !isRed(h->left->left))
        h = moveRedLeft(h);
    h->left = deleteMin(h->left);
    
    return h=fixUp(h);
}

template<class value_type>
typename Set<value_type>::Node * Set<value_type>::deleteIn(Node *h, value_type key) {
    
    if(h==nullptr)  return nullptr;
    if(key<h->key) {
        if(h->left!=nullptr && !isRed(h->left) && !isRed(h->left->left))
            h = moveRedLeft(h);
        h->left = deleteIn(h->left, key);
    }
    else {
        if(isRed(h->left)) {
            h = rotateRight(h);}
        if(key==h->key && h->right==nullptr) {
            h = deleteList(h);
            delete h;
            number--;
            return nullptr;
        }
        
        if(h->right!=nullptr && !isRed(h->right) &&!isRed(h->right->left))
            h = moveRedRight(h);
        if(key==h->key) {
            h->key = getMin(h->right);
            h->right = deleteMin(h->right);
        }
        else    h->right = deleteIn(h->right, key);
        
    }
    
    return h=fixUp(h);
} 

template<class value_type>
size_type Set<value_type>::ifFind(value_type key) {
    Node *h=root;
    while(h!=nullptr) {
        if(key==h->key)  return 1;
        else if(key<h->key)  h = h->left;
        else    h = h->right;
    }
    return 0;
}

template<class value_type>
void Set<value_type>::dedddbug(Node *h) {
    if(h==nullptr)  return;
    printf("@%14p, left %14p, right %14p, color %d, value %d\n", h, h->left, h->right, h->color, h->key);
    dedddbug(h->left);
    dedddbug(h->right);
}

template<class value_type>
void Set<value_type>::insert(const value_type& value) {
    root = insert(root, nullptr, 0, value);
    root->color = BLACK;
}

template<class value_type>
bool Set<value_type>::empty() const{
    return number==0;
}

template<class value_type>
size_type Set<value_type>::size() const{
    return number;
}

template<class value_type>
void Set<value_type>::clear() {
    while(root!=nullptr) {
        root = deleteIn(root, root->key);
    }
    root = nullptr;
}


template<class value_type>
size_type Set<value_type>::erase(const value_type& value) {
    size_type tmp=number;
    if(root!=nullptr) {
        if(!isRed(root->left) && !isRed(root->right))   root->color = RED;
        root = deleteIn(root, value);
        if(root!=nullptr)
            root->color = BLACK;
    }
    return tmp-number;
}

template<class value_type>
size_type Set<value_type>::count(const value_type& value) {
    return ifFind(value);
}

template<class value_type>
typename Set<value_type>::iterator Set<value_type>::find(const value_type& value) {
    Node *h=root;
    while(h!=nullptr) {
        if(value==h->key)   return iterator(h);
        else if(value<h->key)   h = h->left;
        else    h = h->right;
    }
    return iterator(nullptr);
} 

template<class value_type>
typename Set<value_type>::iterator Set<value_type>::lower_bound(const value_type& value) {
    Node *h=root, *x=nullptr;
    while(h!=nullptr) {
        if(h->key>=value && (x==nullptr || h->key<x->key))  x = h;
        if(value<h->key)    h = h->left;
        else if(value==h->key)  break;
        else    h = h->right;
    }
    return iterator(x);
}

template<class value_type>
typename Set<value_type>::iterator Set<value_type>::upper_bound(const value_type& value) {
    Node *h=root, *x=nullptr;
    while(h!=nullptr) {
        if(h->key>value && (x==nullptr || h->key<x->key))  x = h;
        if(value<h->key)    h = h->left;
        else    h = h->right;
    }
    return iterator(x);
}

#endif
