#ifndef INDEX_H
#define INDEX_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include "object.h"

#define MAX_INDEX_ENTRIES 1024

// ==============================
// DATA STRUCTURES
// ==============================

// One file entry in index
typedef struct {
    char path[256];       // file path
    ObjectID hash;        // object hash
    uint32_t mode;        // file mode
    time_t mtime_sec;     // last modified time
    size_t size;          // file size
} IndexEntry;

// Index (staging area)
typedef struct {
    IndexEntry entries[MAX_INDEX_ENTRIES];
    size_t count;
} Index;

// ==============================
// FUNCTION DECLARATIONS
// ==============================

IndexEntry* index_find(Index *index, const char *path);
int index_remove(Index *index, const char *path);
int index_status(const Index *index);
int index_load(Index *index);
int index_save(const Index *index);
int index_add(Index *index, const char *path);

#endif
