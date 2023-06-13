#ifndef AVLTREE_H
#define AVLTREE_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct AVLNode {
    uint32_t depth = 0;
    uint32_t cnt = 0;
    AVLNode *left = NULL;
    AVLNode *right = NULL;
    AVLNode *parent = NULL;
};

#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })

inline void avl_init(AVLNode *node) {
    node->depth = 1;
    node->cnt = 1;
    node->left = node->right = node->parent = NULL;
}

AVLNode *avl_fix(AVLNode *node);
AVLNode *avl_del(AVLNode *node);
AVLNode *avl_offset(AVLNode *node, int64_t offset);

#endif // AVLTREE_H
