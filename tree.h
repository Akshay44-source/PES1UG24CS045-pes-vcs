#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include "object.h"

#define MAX_TREE_ENTRIES 128

typedef struct {
    unsigned int mode;
    char name[256];
    ObjectID hash;
} TreeEntry;

typedef struct {
    TreeEntry entries[MAX_TREE_ENTRIES];
    size_t count;
} Tree;

int tree_serialize(const Tree *tree, void **data_out, size_t *len_out);

#endif
