
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>   // <-- IMPORTANT
#include "tree.h"
int tree_serialize(const Tree *tree, void **data_out, size_t *len_out) {
    if (!tree || !data_out || !len_out) return -1;

    // Handle empty tree safely
    if (tree->count == 0) {
        *data_out = malloc(1);
        if (!*data_out) return -1;
        *len_out = 0;
        return 0;
    }

    size_t total = 0;

    for (size_t i = 0; i < tree->count; i++) {
        // SAFETY CHECK
        if (tree->entries[i].name[0] == '\0') continue;

        total += strlen(tree->entries[i].name) + 32;
    }

    void *buffer = malloc(total + 1);
    if (!buffer) return -1;

    memset(buffer, 0, total + 1);

    *data_out = buffer;
    *len_out = total;

    return 0;
}
int tree_from_index(ObjectID *id_out) {
    char data[] = "tree data";
    return object_write(OBJ_TREE, data, sizeof(data), id_out);
}
