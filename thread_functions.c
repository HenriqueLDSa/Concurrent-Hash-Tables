#include "thread_functions.h"
#include "hash_table.h"
#include "rwlock.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

extern FILE* out;

void* insert_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    insert(record->name, record->salary);
    return NULL;
}

void* search_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    hashRecord* res = search(record->name);
    if (res != NULL) {
        fprintf(out, "%lld: Record found: %s, %d\n", current_timestamp(), res->name, res->salary);
    }
    else {
        fprintf(out, "%lld: No Record Found\n", current_timestamp());
    }
    return NULL;
}

void* delete_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    delete(record->name);
    return NULL;
}

void* print_t(void* arg) {
    print_table();
    return NULL;
}
