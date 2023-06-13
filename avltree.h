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

static void avl_init(AVLNode *node);
static uint32_t avl_depth(AVLNode *node);
static uint32_t avl_cnt(AVLNode *node);
static uint32_t max(uint32_t lhs, uint32_t rhs);
static void avl_update(AVLNode *node);
static AVLNode *rot_left(AVLNode *node);
static AVLNode *rot_right(AVLNode *node);
static AVLNode *avl_fix_left(AVLNode *root);
static AVLNode *avl_fix_right(AVLNode *root);
static AVLNode *avl_fix(AVLNode *node);
static AVLNode *avl_del(AVLNode *node);
static AVLNode *avl_offset(AVLNode *node, int64_t offset);

#endif // AVLTREE_H
