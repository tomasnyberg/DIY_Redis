#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <set>

#include "avltree.cpp" // lazy

using namespace std;

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) ); })

struct Data {
    AVLNode node;
    uint32_t val = 0;
};

struct Container {
    AVLNode *root = NULL;
};

static void add(Container &c, uint32_t val) {
    Data *data = new Data();
    avl_init(&data->node);
    data->val = val;

    if (!c.root) {
        c.root = &data->node;
        return;
    }

    AVLNode *cur = c.root;
    while (true) {
        AVLNode **from =
            (val < container_of(cur, Data, node)->val)
                ? &cur->left
                : &cur->right;
        if (!*from) {
            *from = &data->node;
            data->node.parent = cur;
            c.root = avl_fix(&data->node);
            break;
        }
        cur = *from;
    }
}

static bool del(Container &c, uint32_t val) {
    AVLNode *cur = c.root;
    while (cur) {
        uint32_t node_val = container_of(cur, Data, node)->val;
        if (val == node_val) {
            break;
        }
        cur = val < node_val ? cur->left : cur->right;
    }
    if (!cur) return false;
    c.root = avl_del(cur);
    delete container_of(cur, Data, node);
    return true;
}

static void avl_verify(AVLNode *parent, AVLNode *node) {
    if (!node) return;
    assert(node->parent == parent);
    avl_verify(node, node->left);
    avl_verify(node, node->right);
    assert(node->cnt == 1 + avl_cnt(node->left) + avl_cnt(node->right));
    uint32_t l = avl_depth(node->left);
    uint32_t r = avl_depth(node->right);
    assert(l == r || l == r + 1 || l == r - 1);
    assert(node->depth == 1 + max(l, r));
    uint32_t val = container_of(node, Data, node)->val;
    if (node->left) {
        assert(node->left->parent == node);
        assert(container_of(node->left, Data, node)->val <= val);
    }
    if (node->right) {
        assert(node->right->parent == node);
        assert(container_of(node->right, Data, node)->val >= val);
    }
}

static void extract(AVLNode *node, multiset<uint32_t> &extracted) {
    if (!node) return;
    extract(node->left, extracted);
    extracted.insert(container_of(node, Data, node)->val);
    extract(node->right, extracted);
}

static void container_verify(Container &c, const multiset<uint32_t> &ref) {
    avl_verify(NULL, c.root);
    assert(avl_cnt(c.root) == ref.size());
    multiset<uint32_t> extracted;
    extract(c.root, extracted);
    assert(extracted == ref);
}

static void dispose(Container &c) {
    while (c.root) {
        AVLNode *node = c.root;
        c.root = avl_del(c.root);
        delete container_of(node, Data, node);
    }
}

static void test_insert(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            if (i == val) continue;
            add(c, i);
            ref.insert(i);
        }
        container_verify(c, ref);
        add(c, val);
        ref.insert(val);
        container_verify(c, ref);
        dispose(c);
    }
}

static void test_insert_dup(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            add(c, val);
            ref.insert(val);
        }
        container_verify(c, ref);
        add(c, val);
        ref.insert(val);
        container_verify(c, ref);
        dispose(c);
    }
}

static void test_remove(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            add(c, i);
            ref.insert(i);
        }
        container_verify(c, ref);
        assert(del(c, val));
        ref.erase(val);
        container_verify(c, ref);
        dispose(c);
    }
}