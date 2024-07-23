#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include "sys/time.h"

// Define the name of the files
static const char FILENAME[] = "commands.txt";
static const char OUTPUT_FILENAME[] = "output2.txt";

// Structs
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

// Prototypes
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
hashRecord* search(char* key_name);
long long current_timestamp();
void insert(char* key_name, uint32_t salary);
void delete(char* key_name);
void print_table();
void* insert_t(void* arg);
void* search_t(void* arg);
void* delete_t(void* arg);
void* print_t(void* arg);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
void rwlock_init(rwlock_t* lock);

// Global Variables
FILE* fp;
FILE* out;
rwlock_t mutex;
hashRecord* head = NULL;
int lock_acquisitions = 0;
int lock_releases = 0;
long long curr_time;

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
    char buffer[2][30]; // This just consumes 2 of the strings in
                        // the first line that are not useful
    char command[50];
    int numOfThreads = 0;

    rwlock_init(&mutex);

    // Read first line of the file to get the numOfThreads
    fscanf(fp, "%[^,],%d,%s ", buffer[0], &numOfThreads, buffer[1]);
    fprintf(out, "Running %d threads\n", numOfThreads);

    pthread_t threads[numOfThreads];
    hashRecord* buff = malloc(sizeof(hashRecord));

    // Loop each line of the file
    for (int i = 0; i < numOfThreads; i++)
    {
        fscanf(fp, "%[^,],%[^,],%d ", command, buff->name, &buff->salary);

        if (!strcmp(command, "insert")) {
            pthread_create(&threads[i], NULL, insert_t, buff);
            pthread_join(threads[i], NULL);
        }
        else if (!strcmp(command, "delete")) {
            pthread_create(&threads[i], NULL, delete_t, buff);
            pthread_join(threads[i], NULL);
        }
        else if (!strcmp(command, "search")) {
            pthread_create(&threads[i], NULL, search_t, buff);
            pthread_join(threads[i], NULL);
        }
        else if (!strcmp(command, "print")) {
            pthread_create(&threads[i], NULL, print_t, buff);
            pthread_join(threads[i], NULL);
        }
        else {
            printf("Fatal error reading file commands.");
            return 1;
        }
    }

    fprintf(out, "Finished all threads.\n");
    fprintf(out, "Number of lock acquisitions: %d\n", lock_acquisitions);
    fprintf(out, "Number of lock releases: %d\n", lock_releases);
    print_table();

    free(buff);
    fclose(fp);

    return 0;
}

// Hash Function
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

// Inserts a new key-value pair node or updates an existing one
void insert(char* key_name, uint32_t salary) {
    // Compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    // Print operation
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

    // Uf found, update it; If not, add it to the LL
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

// Deletes key-value pair node if it exists
void delete(char* key_name) {
    // Compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    // Print operation
    curr_time = current_timestamp();
    fprintf(out, "%lld: DELETE,%s\n", curr_time, key_name);

    rwlock_acquire_writelock(&mutex);

    // Search LL for the key
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

// Searches for key-value pair node and if found, returns the value;
// If not found, returns NULL
// If found, the caller prints the record; S
// Otherwise, the caller prints "No Record Found"
hashRecord* search(char* key_name) {
    // Compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    // Print operation
    curr_time = current_timestamp();
    fprintf(out, "%lld: SEARCH,%s\n", curr_time, key_name);

    rwlock_acquire_readlock(&mutex);

    // Search LL for the key
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

void print_table()
{
    // Acquire read-lock
    rwlock_acquire_readlock(&mutex);

    hashRecord* temp = head;
    // Print contents of table
    while(temp != NULL)
    {
        fprintf(out, "%lu,", (unsigned long)temp->hash);
        fprintf(out, "%s,", temp->name);
        fprintf(out, "%d\n", temp->salary);

        temp = temp->next;
    }

    rwlock_release_readlock(&mutex);
}

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
   } else {
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

    sem_post(&lock->lock);
    lock_acquisitions++;

    fprintf(out, "%lld: READ LOCK ACQUIRED\n", current_timestamp());
}

void rwlock_release_readlock(rwlock_t* lock) {
    sem_wait(&lock->lock);
    lock->readers--;

    if (lock->readers == 0)
        sem_post(&lock->writelock);

    sem_post(&lock->lock);
    lock_releases++;

    fprintf(out, "%lld: READ LOCK RELEASED\n", current_timestamp());
}

void rwlock_acquire_writelock(rwlock_t* lock) {
    sem_wait(&lock->writelock);
    lock_acquisitions++;

    fprintf(out, "%lld: WRITE LOCK ACQUIRED\n", current_timestamp());
}

void rwlock_release_writelock(rwlock_t* lock) {
    sem_post(&lock->writelock);
    lock_releases++;

    fprintf(out, "%lld: WRITE LOCK RELEASED\n", current_timestamp());
}

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // Get current time
    long long microseconds = (te.tv_sec * 1000000) + te.tv_usec; // Calculate milliseconds

    return microseconds;
}
