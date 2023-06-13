#ifndef SSET_H
#define SSET_H

#include "avltree.h"
#include "hashtable.h"

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

struct HKey {
    HNode node;
    const char *name = NULL;
    size_t len = 0;
};

#endif