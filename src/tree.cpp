#include "tree.hpp"

template <class T> bool traversal_state<T>::next() {
    // preorder
    T node = NULL;
    while (!node) {
        if (m_stack_pointer < 0 || !m_stack[m_stack_pointer].node_pointer)
            return false;
        if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0 ||
            m_stack[m_stack_pointer].leaf_pointer >=
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
            node = m_stack[m_stack_pointer].node_pointer->get_data();
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
    m_item = node;
    return true;
}

template <class T>
traversal_state<T>
tree_node<T>::get_traversal_state(typename traversal_state<T>::MODE mode) {
    return traversal_state<T>(mode, this);
};

template <class T> void tree_node<T>::insert_node(tree_node<T>* node, int idx) {
    if (idx >= 0 && idx <= children.size()) {
        children.insert(children.begin() + idx, node);
        return;
    }
    children.push_back(node);
    return;
}

template <class T> tree_node<T>* tree_node<T>::create_new(T data) {
    return new tree_node<T>(data);
}

template <class T> int tree_node<T>::size(tree_node<T>* node) {
    int size = 0;
    if (node) {
        size += 1;
        for (int i = 0; i < node->children.size(); i++)
            size += tree_node<T>::size(node->children[i]);
    }
    return size;
}

template <class T> void tree_node<T>::destroy(tree_node<T>* node) {
    if (node->data)
        delete node->data;
    for (int i = 0; i < node->children.size(); i++)
        tree_node<T>::destroy(node->children[i]);
    delete node;
}

