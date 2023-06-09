#include "avltree.h" 
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

struct AVLNode {
    uint32_t depth = 0;
    uint32_t cnt = 0;
    AVLNode *left = NULL;
    AVLNode *right = NULL;
    AVLNode *parent = NULL;
};

static void avl_init(AVLNode *node) {
    node->depth = 1;
    node->cnt = 1;
    node->left = node->right = node->parent = NULL;
}

static uint32_t avl_depth(AVLNode *node){
    return node ? node->depth : 0;
}

static uint32_t avl_cnt(AVLNode *node){
    return node ? node->cnt : 0;
}

static uint32_t max(uint32_t lhs, uint32_t rhs){
    return lhs < rhs ? rhs:lhs;
}

static void avl_update(AVLNode *node){
    node->depth = max(avl_depth(node->left), avl_depth(node->right)) + 1;
    node->cnt = avl_cnt(node->left) + avl_cnt(node->right) + 1;
}