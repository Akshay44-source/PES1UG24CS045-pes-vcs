#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>

// ---------- helpers ----------

// check if file exists
static int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

// create directory if not exists
static void ensure_dir(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0777);
    }
}

// ---------- WRITE OBJECT ----------

int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out) {
    (void)type; // currently unused

    // compute SHA-256
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, len);
    SHA256_Final(id_out->hash, &ctx);

    // create directories
    ensure_dir(".pes");
    ensure_dir(".pes/objects");

    char dir[256];
    snprintf(dir, sizeof(dir), ".pes/objects/%02x", id_out->hash[0]);
    ensure_dir(dir);

    // create full file path
    char path[512];
    snprintf(path, sizeof(path),
             "%s/%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             dir,
             id_out->hash[1], id_out->hash[2], id_out->hash[3],
             id_out->hash[4], id_out->hash[5], id_out->hash[6],
             id_out->hash[7], id_out->hash[8], id_out->hash[9],
             id_out->hash[10], id_out->hash[11], id_out->hash[12],
             id_out->hash[13], id_out->hash[14], id_out->hash[15],
             id_out->hash[16], id_out->hash[17], id_out->hash[18],
             id_out->hash[19], id_out->hash[20], id_out->hash[21],
             id_out->hash[22], id_out->hash[23], id_out->hash[24],
             id_out->hash[25], id_out->hash[26], id_out->hash[27],
             id_out->hash[28], id_out->hash[29], id_out->hash[30],
             id_out->hash[31]);

    // deduplication
    if (file_exists(path)) {
        return 0;
    }

    // write file
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("fopen");
        return -1;
    }

    if (fwrite(data, 1, len, f) != len) {
        perror("fwrite");
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

// ---------- READ OBJECT ----------

int object_read(const ObjectID *id, ObjectType *type_out,
                void **data_out, size_t *len_out) {

    (void)type_out; // unused

    char path[512];
    snprintf(path, sizeof(path),
             ".pes/objects/%02x/%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             id->hash[0],
             id->hash[1], id->hash[2], id->hash[3],
             id->hash[4], id->hash[5], id->hash[6],
             id->hash[7], id->hash[8], id->hash[9],
             id->hash[10], id->hash[11], id->hash[12],
             id->hash[13], id->hash[14], id->hash[15],
             id->hash[16], id->hash[17], id->hash[18],
             id->hash[19], id->hash[20], id->hash[21],
             id->hash[22], id->hash[23], id->hash[24],
             id->hash[25], id->hash[26], id->hash[27],
             id->hash[28], id->hash[29], id->hash[30],
             id->hash[31]);

    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("fopen");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    void *data = malloc(size);
    if (!data) {
        fclose(f);
        return -1;
    }

    if (fread(data, 1, size, f) != (size_t)size) {
        perror("fread");
        free(data);
        fclose(f);
        return -1;
    }

    fclose(f);

    *data_out = data;
    *len_out = size;

    return 0;
}

// ---------- HASH HELPERS ----------

// ObjectID → hex
void hash_to_hex(const ObjectID *id, char *hex_out) {
    for (int i = 0; i < 32; i++) {
        sprintf(hex_out + i * 2, "%02x", id->hash[i]);
    }
    hex_out[64] = '\0';
}

// hex → ObjectID
int hex_to_hash(const char *hex, ObjectID *id_out) {
    for (int i = 0; i < 32; i++) {
        if (sscanf(hex + 2 * i, "%2hhx", &id_out->hash[i]) != 1) {
            return -1;
        }
    }
    return 0;
}

