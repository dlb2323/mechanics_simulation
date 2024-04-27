#ifndef TREE_H
#define TREE_H

#include <vector>
#include <iostream>

template <typename T> class tree_node;

template <typename T> class traversal_state {
    typedef struct {
        tree_node<T>* node_pointer;
        int leaf_pointer;
    } state;
    state m_stack[128];
    int m_stack_pointer;
    T m_item;

public:
    enum MODE { PREORDER, POSTORDER, INORDER };
    const MODE mode;
    traversal_state(MODE mode, tree_node<T>* root) : mode(mode) {
        m_stack[0] = state{ root, 0 };
        m_stack_pointer = 0;
    };
    T get_item() const { return m_item; };
    bool next();
};

template <typename T> class tree_node {
    T data;
    bool lock = false;
    std::vector<tree_node<T>*> children;

    tree_node<T>(T data) { tree_node<T>::data = data; }

public:
    static tree_node<T>* create_new(T data);
    static int size(tree_node<T>* node);
    static void destroy(tree_node<T>* node);
    traversal_state<T>
        get_traversal_state(typename traversal_state<T>::MODE mode);
    void insert_node(tree_node<T>* node, int idx = -1);

    T get_data() const { return data; }
    tree_node<T>* get_child(int idx) const { return children[idx]; }
    unsigned int get_child_count() const { return children.size(); }
};

template <typename T> 
inline bool traversal_state<T>::next() {
    bool node_found = false;
    while (!node_found) {
        if (m_stack_pointer < 0)
            return false;
        if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0 ||
            m_stack[m_stack_pointer].leaf_pointer >=
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
            m_item = m_stack[m_stack_pointer].node_pointer->get_data();
            node_found = true;
            m_stack_pointer--;
        }
        else {
            m_stack[m_stack_pointer + 1] =
                state{ m_stack[m_stack_pointer].node_pointer->get_child(
                    m_stack[m_stack_pointer].leaf_pointer),
                      0 };
            m_stack[m_stack_pointer].leaf_pointer++;
            m_stack_pointer++;
        }
    }
    return true;
}

template <typename T>
inline traversal_state<T>
tree_node<T>::get_traversal_state(typename traversal_state<T>::MODE mode) {
    return traversal_state<T>(mode, this);
};

template <typename T> 
inline void tree_node<T>::insert_node(tree_node<T>* node, int idx) {
    if (idx >= 0 && idx <= children.size()) {
        children.insert(children.begin() + idx, node);
        return;
    }
    children.push_back(node);
    return;
}

template <typename T> 
inline tree_node<T>* tree_node<T>::create_new(T data) {
    return new tree_node<T>(data);
}

template <typename T> 
inline int tree_node<T>::size(tree_node<T>* node) {
    int size = 0;
    if (node) {
        size += 1;
        for (int i = 0; i < node->children.size(); i++)
            size += tree_node<T>::size(node->children[i]);
    }
    return size;
}

template <typename T> 
inline void tree_node<T>::destroy(tree_node<T>* node) {
    if (node->data)
        delete node->data;
    for (int i = 0; i < node->children.size(); i++)
        tree_node<T>::destroy(node->children[i]);
    delete node;
}

#endif // !TREE_H
