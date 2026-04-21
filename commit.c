#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commit.h"
#include "object.h"
#include "tree.h"

int commit_create(const char *message, ObjectID *commit_id_out) {

    // STEP 1: Create tree from index
    ObjectID tree_id;
    if (tree_from_index(&tree_id) != 0) {
        printf("Error: tree creation failed\n");
        return -1;
    }

    // STEP 2: Build commit structure
    Commit commit;
    memset(&commit, 0, sizeof(Commit));

    commit.tree = tree_id;
    commit.has_parent = 0;

    strcpy(commit.author, "Akshay");   // your name
    commit.timestamp = time(NULL);
    strncpy(commit.message, message, sizeof(commit.message)-1);

    // STEP 3: Serialize commit
    void *data = NULL;
    size_t len = 0;

    if (commit_serialize(&commit, &data, &len) != 0) {
        printf("Error: commit serialize failed\n");
        return -1;
    }

    // STEP 4: WRITE OBJECT (MOST IMPORTANT)
    if (object_write(OBJ_COMMIT, data, len, commit_id_out) != 0) {
        printf("Error: object write failed\n");
        free(data);
        return -1;
    }

    free(data);

    // STEP 5: Print output
    printf("Committed: %s\n", message);

    return 0;
}
int commit_serialize(const Commit *commit, void **data_out, size_t *len_out) {
    char buffer[512];

    int len = snprintf(buffer, sizeof(buffer),
        "author %s\nmessage %s\n",
        commit->author,
        commit->message);

    *data_out = malloc(len);
    memcpy(*data_out, buffer, len);
    *len_out = len;

    return 0;
}

