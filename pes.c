#include <time.h>
#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <time.h>

#define INDEX_FILE ".pes/index"
#define LOG_FILE ".pes/log"

/* ---------- INIT ---------- */
void cmd_init() {
    mkdir(".pes", 0777);
    mkdir(".pes/objects", 0777);
    mkdir(".pes/refs", 0777);

    FILE *f = fopen(".pes/HEAD", "w");
    if (f) {
        fprintf(f, "ref: refs/heads/main\n");
        fclose(f);
    }

    printf("Initialized empty PES repository in .pes\n");
}

/* ---------- HASH ---------- */
void get_file_hash(const char *filename, char *output) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return;

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    unsigned char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        SHA256_Update(&ctx, buffer, bytes);
    }

    unsigned char hash[32];
    SHA256_Final(hash, &ctx);

    for (int i = 0; i < 32; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    fclose(fp);
}

/* ---------- ADD ---------- */
void cmd_add(int argc, char *argv[]) {
    FILE *fp = fopen(INDEX_FILE, "a");
    if (!fp) {
        printf("Repo not initialized\n");
        return;
    }

    for (int i = 2; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) != 0) continue;

        char hash_str[65] = {0};
        get_file_hash(argv[i], hash_str);

        fprintf(fp, "100644 %s %ld %ld %s\n",
                hash_str,
                st.st_mtime,
                st.st_size,
                argv[i]);

        printf("Added file: %s\n", argv[i]);
    }

    fclose(fp);
}

/* ---------- STATUS ---------- */
int is_tracked(const char *filename) {
    FILE *fp = fopen(INDEX_FILE, "r");
    if (!fp) return 0;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char *name = strrchr(line, ' ');
        if (name) {
            name++;
            name[strcspn(name, "\n")] = 0;
            if (strcmp(name, filename) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

void cmd_status() {
    printf("Staged changes:\n");

    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            printf("  %s", strrchr(line, ' ') + 1);
        }
        fclose(fp);
    }

    printf("\nUnstaged changes:\n  (nothing to show)\n");

    printf("\nUntracked files:\n");

    FILE *ls = popen("ls", "r");
    char file[256];
    while (fgets(file, sizeof(file), ls)) {
        file[strcspn(file, "\n")] = 0;
        if (strcmp(file, ".pes") == 0) continue;
        if (!is_tracked(file)) printf("  %s\n", file);
    }
    pclose(ls);
}

/* ---------- COMMIT ---------- */
void cmd_commit(char *msg) {
    FILE *fp = fopen(".pes/log", "a");
    if (!fp) {
        printf("Repo not initialized\n");
        return;
    }

    // Generate fake commit hash (using time)
    char hash[65];
    sprintf(hash, "%lx%lx", time(NULL), random());

    time_t now = time(NULL);

    fprintf(fp, "commit %s\n", hash);
    fprintf(fp, "Author: Akshay Kumar M <PES1UGXX>\n");
    fprintf(fp, "Date:   %ld\n\n", now);
    fprintf(fp, "    %s\n\n", msg);

    fclose(fp);

    printf("Committed: %s\n", msg);
}

/* ---------- LOG ---------- */
void cmd_log() {
    FILE *fp = fopen(".pes/log", "r");
    if (!fp) {
        printf("No commits yet\n");
        return;
    }

    char lines[1000][256];
    int count = 0;

    while (fgets(lines[count], sizeof(lines[count]), fp)) {
        count++;
    }
    fclose(fp);

    // Print in reverse (latest first)
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", lines[i]);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./pes <command>\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        cmd_init();
    } 
    else if (strcmp(argv[1], "add") == 0) {
        cmd_add(argc, argv);
    } 
    else if (strcmp(argv[1], "status") == 0) {
        cmd_status();
    } 
    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Commit message required\n");
            return 1;
        }
        cmd_commit(argv[2]);
    } 
    else if (strcmp(argv[1], "log") == 0) {
        cmd_log();
    } 
    else {
        printf("Unknown command\n");
    }

    return 0;
}
