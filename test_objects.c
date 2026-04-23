#include "object.h"
#include <stdio.h>
#include <string.h>

int main() {
    const char *buffer = "hello world";
    ObjectID id;

    if (object_write(OBJ_BLOB, buffer, strlen(buffer), &id) != 0) {
        printf("write failed\n");
        return 1;
    }

    printf("Object written with hash:\n");
    for (int i = 0; i < 32; i++) {
        printf("%02x", id.hash[i]);
    }
    printf("\n");

    return 0;
}
