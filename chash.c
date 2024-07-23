#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "hash_table.h"
#include "thread_functions.h"
#include "rwlock.h"

static const char FILENAME[] = "commands.txt";
static const char OUTPUT_FILENAME[] = "output.txt";

FILE* fp;
FILE* out;
rwlock_t mutex;
hashRecord* head = NULL;
int lock_acquisitions = 0;
int lock_releases = 0;

int main() {
    fp = fopen(FILENAME, "r");
    out = fopen(OUTPUT_FILENAME, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: File \"%s\" not found.\n", FILENAME);
        exit(EXIT_FAILURE);
    }

    if (out == NULL) {
        fprintf(stderr, "Error opening file \"%s\".\n", OUTPUT_FILENAME);
        exit(EXIT_FAILURE);
    }

    char buffer[2][30];
    char command[50];
    int numOfThreads = 0;

    rwlock_init(&mutex);

    fscanf(fp, "%[^,],%d,%s ", buffer[0], &numOfThreads, buffer[1]);
    fprintf(out, "Running %d threads\n", numOfThreads);

    pthread_t threads[numOfThreads];
    hashRecord* buff = malloc(sizeof(hashRecord));

    for (int i = 0; i < numOfThreads; i++) {
        fscanf(fp, "%[^,],%[^,],%d ", command, buff->name, &buff->salary);

        if (strcmp(command, "insert") == 0) {
            pthread_create(&threads[i], NULL, insert_t, buff);
        }
        else if (strcmp(command, "delete") == 0) {
            pthread_create(&threads[i], NULL, delete_t, buff);
        }
        else if (strcmp(command, "search") == 0) {
            pthread_create(&threads[i], NULL, search_t, buff);
        }
        else if (strcmp(command, "print") == 0) {
            pthread_create(&threads[i], NULL, print_t, NULL);
        }
        else {
            free(buff);
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        pthread_join(threads[i], NULL);
    }

    fprintf(out, "Finished all threads.\n");
    fprintf(out, "Number of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(out, "Number of lock releases: %d\n", lock_releases);
    print_table();

    free(buff);
    fclose(fp);
    fclose(out);

    return 0;
}
