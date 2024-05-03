#ifndef TREE_H
#define TREE_H

#include <vector>
#include <iostream>

// forward declare tree
template <typename T> class tree_node;

// generic traversal instance
// used to traverse a tree iteratively without access to tree data
template <typename T> class traversal_state {
    // stack state object 
    // holds a node and a leaf pointer to indicate which child is being pointed to
    typedef struct {
        tree_node<T>* node_pointer;
        int leaf_pointer;
    } state;
    // state stack
    // max traversal depth 128
    state m_stack[128];
    // points to the current stack state
    int m_stack_pointer;
    // item returned by get_item()
    T m_item;
    // current stack pointer for use by leave_branch()
    int m_item_stack_pointer;
    // preorder traversal
    bool next_preorder();
    // postorder traversal
    bool next_postorder();

public:
    // traversal mode
    enum MODE { PREORDER, POSTORDER, INORDER };
    // store traversal mode as constant
    const MODE mode;
    traversal_state(MODE mode, tree_node<T>* root) : mode(mode) {
        // initialise stack with root and pointing at first leaf
        m_stack[0] = state{ root, 0 };
        // initialise stack pointer
        m_stack_pointer = 0;
    };
    // item getter
    T get_item() const { return m_item; };
    // gets the current node
    tree_node<T>* get_node() const {
        tree_node<T>* p_node = NULL;
        if (m_stack_pointer > 0)
            p_node = m_stack[m_stack_pointer].node_pointer;
        return p_node;
    };
    // moves state to next item
    bool next();
    // skips a node during preorder traversal
    void leave_branch();
};

// tree implementation
// templated to simplify testing
template <typename T> class tree_node {
    // parent pointer
    // NULL indicates that this node is a root
    tree_node<T>* parent = NULL;
    // store data type
    T data;
    // vector containing children
    std::vector<tree_node<T>*> children;

    // private constructor
    tree_node<T>(T data) { tree_node<T>::data = data; }

public:
    // static creation method
    static tree_node<T>* create_new(T data);
    // return total number of nodes in tree
    static int size(tree_node<T>* node);
    // delete a tree 
    static void destroy(tree_node<T>* node);
    // returns a traversal state for iteration 
    traversal_state<T>
        get_traversal_state(typename traversal_state<T>::MODE mode);
    // insert a node
    // if no leaf index is given, the node is inserted as the last leaf
    void insert_node(tree_node<T>* node, int idx = -1);

    // data getter
    T get_data() const { return data; }
    // return parent 
    tree_node<T>* get_parent() const { return parent; }
    // return child at index 
    tree_node<T>* get_child(int idx) const { return children[idx]; }
    // return number of children
    unsigned int get_child_count() const { return children.size(); }
};

// calls specific traversal function based on traversal mode
// otherwise, returns false ending a while(next()) loop
template <typename T> 
inline bool traversal_state<T>::next() {
    switch(mode) {
        case MODE::PREORDER:
            return next_preorder();
            break;
        case MODE::POSTORDER:
            return next_postorder();
            break;
        case MODE::INORDER:
            break;
    }
    return false;
}

// return next item using post order traversal
// traversal in progress -> true, traversal complete -> false
template <typename T> 
inline bool traversal_state<T>::next_postorder() {
    bool node_found = false;
    // loop until valid node is located
    while (!node_found) {
        // stack pointer of -1 indicates that traversal has finished
        if (m_stack_pointer < 0)
            return false;
        if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0 ||
            m_stack[m_stack_pointer].leaf_pointer >=
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
            // if node has no children or a non-existent leaf is accessed
            // max depth reached
            m_item = m_stack[m_stack_pointer].node_pointer->get_data();
            node_found = true;
            // revert to previous stack state
            m_stack_pointer--;
        }
        else {
            // node still has valid leaves to traverse
            // create new stack entry
            m_stack[m_stack_pointer + 1] =
                state{ m_stack[m_stack_pointer].node_pointer->get_child(
                    m_stack[m_stack_pointer].leaf_pointer),
                      0 };
            // point to next leaf with current stack state
            m_stack[m_stack_pointer].leaf_pointer++;
            // point to the new entry
            m_stack_pointer++;
        }
    }
    // node found, return success
    return true;
}

// return next item using preorder traversal
// traversal in progress -> true, traversal complete -> false
template<typename T>
inline bool traversal_state<T>::next_preorder() {
    bool node_found = false;
    // loop until valid node is located
    while(!node_found) {
        // stack pointer of -1 indicates that traversal has finished
        if (m_stack_pointer < 0)
            return false;
        // store item from current node
        m_item = m_stack[m_stack_pointer].node_pointer->get_data();
        // store stack pointer state for leave_branch() to access 
        m_item_stack_pointer = m_stack_pointer;
        // check if the current leaf pointer is less than the total number of leaves
        // check fails if total leaves is 0 or leaf pointer is too large
        if (m_stack[m_stack_pointer].leaf_pointer <
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
            // create new stack entry for leaf 
            m_stack[m_stack_pointer+1] = state{ m_stack[m_stack_pointer].node_pointer->get_child(
                m_stack[m_stack_pointer].leaf_pointer), 0 };
            if (m_stack[m_stack_pointer].leaf_pointer == 0)
                // first access, return node 
                node_found = true;
            // point to next leaf
            m_stack[m_stack_pointer].leaf_pointer++;
            // move to new stack entry 
            m_stack_pointer++;
        }
        else {
            if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0)
                // no leaves so first check will always fail
                // the node must be traversed so return here
                node_found = true;
            // revert to previous stack entry 
            m_stack_pointer--;
        }
    }
    // node found, return success
    return true;
}

// during preorder traversal, skipping the current node will omit the entire branch
template <typename T> 
inline void traversal_state<T>::leave_branch() {
    if (mode == PREORDER)
        // overwrites the stack pointer to the parent of the current node
        m_stack_pointer=m_item_stack_pointer-1;
}

// returns a traversal instance based on the current node
template <typename T>
inline traversal_state<T>
tree_node<T>::get_traversal_state(typename traversal_state<T>::MODE mode) {
    return traversal_state<T>(mode, this);
};

// inserts node into tree at index
template <typename T> 
inline void tree_node<T>::insert_node(tree_node<T>* node, int idx) {
    // checks if the index is provide and is valid
    if (idx >= 0 && idx <= children.size()) {
        // insert at index 
        children.insert(children.begin() + idx, node);
        return;
    }
    node->parent = this;
    // insert at position
    children.push_back(node);
    return;
}

// create tree node with data
template <typename T> 
inline tree_node<T>* tree_node<T>::create_new(T data) {
    return new tree_node<T>(data);
}

// recursively count the nodes in the tree
template <typename T> 
inline int tree_node<T>::size(tree_node<T>* node) {
    int size = 0;
    // check if node exists 
    if (node) {
        size += 1;
        // add up the sizes of child nodes 
        for (int i = 0; i < node->children.size(); i++)
            size += tree_node<T>::size(node->children[i]);
    }
    return size;
}

// recursively delete tree 
template <typename T> 
inline void tree_node<T>::destroy(tree_node<T>* node) {
    // if parent exists, remove from parent's children 
    if (node->parent)
        node->parent->children.erase(std::find(node->parent->children.begin(), node->parent->children.end(), node));
    // delete node data 
    if (node->data)
        delete node->data;
    // delete children
    for (int i = 0; i < node->children.size(); i++)
        tree_node<T>::destroy(node->children[i]);
    // delete current node
    delete node;
}

#endif // !TREE_H
