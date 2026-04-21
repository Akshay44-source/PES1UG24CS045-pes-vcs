#ifndef COMMIT_H
#define COMMIT_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include "object.h"

// ==============================
// CONSTANTS
// ==============================


#define PES_DIR ".pes"
#define HEAD_FILE ".pes/HEAD"
#define REFS_DIR ".pes/refs"
// ==============================
// DATA STRUCTURES
// ==============================

// Commit object
typedef struct {
    ObjectID tree;          // root tree
    int has_parent;         // 0 or 1
    ObjectID parent;        // parent commit
    char author[128];       // author name
    time_t timestamp;       // commit time
    char message[256];      // commit message
} Commit;

// Function pointer for commit walk
typedef void (*commit_walk_fn)(const ObjectID *id, const Commit *commit, void *ctx);

// ==============================
// FUNCTION DECLARATIONS
// ==============================

int commit_parse(const void *data, size_t len, Commit *commit_out);
int commit_serialize(const Commit *commit, void **data_out, size_t *len_out);

int commit_create(const char *message, ObjectID *commit_id_out);

int commit_walk(commit_walk_fn callback, void *ctx);

int head_read(ObjectID *id_out);
int head_update(const ObjectID *new_commit);

#endif
