#ifndef TREE_H
#define TREE_H

#include <vector>

template <class T> class tree_node;

template <class T> class traversal_state {
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
    };
    T get_item() const { return m_item; };
    bool next();
};

template <class T> class tree_node {
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

#endif // !TREE_H
