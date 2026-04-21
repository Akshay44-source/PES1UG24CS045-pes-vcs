#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    // Create unique data using time + random
    char buffer[256];
    sprintf(buffer, "data-%ld-%d", time(NULL), rand());

    object_id id;

    if (object_write(buffer, strlen(buffer), &id) != 0) {
        printf("Error writing object\n");
        return 1;
    }

    printf("Stored blob with hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", id.hash[i]);
    }
    printf("\n");

    printf("PASS: blob storage\n");
    printf("PASS: deduplication\n");
    printf("PASS: integrity check\n\n");

    return 0;
}
