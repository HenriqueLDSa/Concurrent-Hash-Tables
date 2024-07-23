#include "hash_table.h"
#include "rwlock.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

extern const char FILENAME[];
extern const char OUTPUT_FILENAME[];

extern FILE* fp;
extern FILE* out;
extern rwlock_t mutex;
extern hashRecord* head;
extern int lock_acquisitions;
extern int lock_releases;
extern long long curr_time;

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;

    while (i != length)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}

void insert(char* key_name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    curr_time = current_timestamp();
    fprintf(out, "%lld: INSERT,%"PRIu32",%s,%d\n", curr_time, hash, key_name, salary);

    rwlock_acquire_writelock(&mutex);

    if (head == NULL)
    {
        hashRecord* newNode = malloc(sizeof(hashRecord));
        strcpy(newNode->name, key_name);
        newNode->salary = salary;
        newNode->hash = hash;

        head = newNode;

        rwlock_release_writelock(&mutex);
        return;
    }

    hashRecord* temp = head;

    while (temp != NULL) {
        if (temp->hash == hash) {
            strcpy(temp->name, key_name);
            temp->salary = salary;
            break;
        }
        else if (temp->next == NULL) {
            hashRecord* newNode = malloc(sizeof(hashRecord));
            strcpy(newNode->name, key_name);
            newNode->salary = salary;
            newNode->hash = hash;

            newNode->next = temp->next;
            temp->next = newNode;
        }

        temp = temp->next;
    }

    rwlock_release_writelock(&mutex);
    return;
}

void delete(char* key_name) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    curr_time = current_timestamp();
    fprintf(out, "%lld: DELETE,%s\n", curr_time, key_name);

    rwlock_acquire_writelock(&mutex);

    hashRecord* temp = head;
    hashRecord* prev = NULL;

    while (temp != NULL)
    {
        if (temp->hash == hash)
        {
            (prev == NULL) ? (head = temp->next) : (prev->next = temp->next);

            hashRecord* nodeToDelete = temp;
            temp = temp->next;
            free(nodeToDelete);
            break;
        }

        prev = temp;
        temp = temp->next;
    }

    rwlock_release_writelock(&mutex);
    return;
}

hashRecord* search(char* key_name) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    curr_time = current_timestamp();
    fprintf(out, "%lld: SEARCH,%s\n", curr_time, key_name);

    rwlock_acquire_readlock(&mutex);

    hashRecord* temp = head;

    while (temp != NULL)
    {
        if (temp->hash == hash)
        {
            rwlock_release_readlock(&mutex);
            return temp;
        }

        temp = temp->next;
    }

    rwlock_release_readlock(&mutex);
    return NULL;
}

void print_table() {

    rwlock_acquire_readlock(&mutex);

    hashRecord* array = linkedList_to_sorted_array(head);
    int length = getLength(head);

    for (int i = 0; i < length; i++) {
        fprintf(out, "%lu,", (unsigned long)array[i].hash);
        fprintf(out, "%s,", array[i].name);
        fprintf(out, "%d\n", array[i].salary);
    }

    rwlock_release_readlock(&mutex);

    free(array);
}
