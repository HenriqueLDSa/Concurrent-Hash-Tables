#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "sys/time.h"

// Define the name of the file to be read HERE
static const char FILENAME[] = "commands.txt";
static const char OUTPUT_FILENAME[] = "output.txt";

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct* next;
} hashRecord;

typedef struct _rwlock_t {
    sem_t writelock;
    sem_t lock;
    int readers;
} rwlock_t;


uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
void insert(char* key_name, uint32_t salary);
void delete(char* key_name);
hashRecord* search(char* key_name);
void* insert_t(void* arg);
void* search_t(void* arg);
void* delete_t(void* arg);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
void rwlock_init(rwlock_t* lock);
long long current_timestamp();

FILE* fp;
FILE* out;

rwlock_t mutex;
hashRecord* head = NULL;
int lock_acquisitions = 0;
int lock_releases = 0;

int main() {
    // Open File
    fp = fopen(FILENAME, "r");
    out = fopen(OUTPUT_FILENAME, "w");

    if (fp == NULL) {
        printf("Error: File \"%s\" not found.", FILENAME);
        exit(1);
    }
    if (out == NULL) {
        printf("Error opening file \"%s\".", OUTPUT_FILENAME);
        exit(1);
    }

    // Variable Declaration
    char buffer[2][30]; // This just consumes 2 of the strings in the first line
                        // that are not useful
    char command[30];
    char name[30];
    int salary = 0;
    int numOfThreads = 0;

    rwlock_init(&mutex);

    // Read first line of the file to get the numOfThreads
    fscanf(fp, "%[^,],%d,%s", buffer[0], &numOfThreads, buffer[1]);

    fprintf(out, "Running %d threads\n", numOfThreads);

    pthread_t threads[numOfThreads];
    hashRecord* buff = malloc(sizeof(hashRecord));

    // Loop each line of the file
    for (int i = 0; i < numOfThreads; i++)
    {
        fscanf(fp, "%[^,],%[^,],%d", command, name, &salary);
        strcpy(buff->name, name);
        buff->salary = salary;

        // EVERYTHING SHOULD BE PROCESSED HERE     
    }

    free(buff);

    // Close file
    fclose(fp);

    return 0;
}

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

//inserts a new key-value pair node or updates an existing one
void insert(char* key_name, uint32_t salary) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    long long curr_time = current_timestamp();

    fprintf(out, "%d: INSERT,%d,%s,%d\n", curr_time, hash, key_name, salary);

    //acquire the writer-lock that protects the list and searches the linked list for the hash
    rwlock_acquire_writelock(&mutex);

    curr_time = current_timestamp();
    fprintf(out, "%d: WRITE LOCK ACQUIRED\n", curr_time);

    if (head == NULL)
    {
        hashRecord* newNode = malloc(sizeof(hashRecord));
        strcpy(newNode->name, key_name);
        newNode->salary = salary;
        newNode->hash = hash;

        head = newNode;

        curr_time = current_timestamp();
        fprintf(out, "%d: WRITE LOCK RELEASED\n", curr_time);
        rwlock_release_writelock(&mutex);

        return;
    }

    //if found, update it; if not, add it to the LL
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

    //release writer-lock
    curr_time = current_timestamp();
    fprintf(out, "%d: WRITE LOCK RELEASED\n", curr_time);
    rwlock_release_writelock(&mutex);

    return;
}

//deletes key-value pair node if it exists
void delete(char* key_name) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    //acquire the writer-lock
    rwlock_acquire_writelock(&mutex);

    //search LL for the key
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

    //release writer-lock and return
    rwlock_release_writelock(&mutex);

    return;
}

//searches for key-value pair node and if found, returns the value; if not found, returns NULL
//if found, the caller prints the record; otherwise, the caller prints "No Record Found"
hashRecord* search(char* key_name) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    // acquire reader-lock
    rwlock_acquire_readlock(&mutex);

    //search LL for the key
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

void* insert_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    insert(record->name, record->salary);
    return NULL;
}

void* search_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    search(record->name);
    return NULL;
}

void* delete_t(void* arg) {
    hashRecord* record = (hashRecord*)arg;
    delete(record->name);
    return NULL;
}

void rwlock_init(rwlock_t* lock) {
    lock->readers = 0;
    sem_init(&lock->lock, 0, 1);
    sem_init(&lock->writelock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t* lock) {
    sem_wait(&lock->lock);
    lock->readers++;
    if (lock->readers == 1)
        sem_wait(&lock->writelock);

    lock_acquisitions++;
    sem_post(&lock->lock);

}

void rwlock_release_readlock(rwlock_t* lock) {
    sem_wait(&lock->lock);
    lock->readers--;
    lock_releases++;
    if (lock->readers == 0)
        sem_post(&lock->writelock);
    sem_post(&lock->lock);
}

void rwlock_acquire_writelock(rwlock_t* lock) {
    sem_wait(&lock->writelock);

    lock_acquisitions++;
}

void rwlock_release_writelock(rwlock_t* lock) {
    lock_releases++;

    sem_post(&lock->writelock);
}

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long microseconds = (te.tv_sec * 1000000) + te.tv_usec; // calculate milliseconds
    return microseconds;
}
