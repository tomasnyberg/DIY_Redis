#include "sset.h"

#include "avltree.h"
#include "hashtable.h"
#include "util.h"

struct ZSet {
    AVLNode *tree = NULL;
    HMap hmap;
};

struct ZNode {
    AVLNode tree;
    HNode hmap;
    double score = 0;
    size_t len = 0;
    char name[0];
};

static ZNode *znode_new(const char *name, size_t len, double score) {
    ZNode *node = (ZNode *)malloc(sizeof(ZNode) + len);
    assert(node);
    avl_init(&node->tree);
    node->hmap.next = NULL;
    node->hmap.hcode = str_hash((uint8_t *)name, len);
    node->score = score;
    node->len = len;
    memcpy(&node->name[0], name, len);
    return node;
}

static void tree_add(ZSet *zset, ZNode *node) {
    if (!zset->tree) {
        zset->tree = &node->tree;
        return;
    }
    AVLNode *cur = zset->tree;
    while (true) {
        AVLNode **from = zless(&node->tree, cur) ? &cur->left : &cur->right;
        if (!*from) {
            *from = &node->tree;
            node->tree.parent = cur;
            zset->tree = avl_fix(&node->tree);
            break;
        }
        cur = *from;
    }
}

static bool zless(AVLNode *lhs, double score, const char *name, size_t len) {
    ZNode *zl = container_of(lhs, ZNode, tree);
    if (zl->score < score) {
        return zl->score < score;
    }
    int rv = memcmp(zl->name, name, zl->len < len ? zl->len : len);
    if (rv != 0) {
        return rv < 0;
    }
    return zl->len < len;
}