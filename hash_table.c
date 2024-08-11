#include "hash_table.h"
#include "rwlock.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>

extern const char FILENAME[];
extern const char OUTPUT_FILENAME[];

extern FILE* fp;
extern FILE* out;
extern rwlock_t mutex;
extern pthread_mutex_t cond_mutex;
extern pthread_cond_t cond;
extern int insert_count;
extern int insert_target;
extern hashRecord* head;
extern int lock_acquisitions;
extern int lock_releases;

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

    fprintf(out, "%lld: INSERT,%"PRIu32",%s,%d\n", current_timestamp(), hash, key_name, salary);

    rwlock_acquire_writelock(&mutex);

    insert_target++;

    pthread_mutex_lock(&cond_mutex);
    insert_count++;
    if (insert_count == insert_target) {
        pthread_cond_signal(&cond); // Signal the division thread
    }
    pthread_mutex_unlock(&cond_mutex);

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

    fprintf(out, "%lld: DELETE,%s\n", current_timestamp(), key_name);

    pthread_mutex_lock(&cond_mutex);
    while (insert_count < insert_target) {
        pthread_cond_wait(&cond, &cond_mutex);
    }
    pthread_mutex_unlock(&cond_mutex);

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

    fprintf(out, "%lld: SEARCH,%s\n", current_timestamp(), key_name);

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
