#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Define the name of the file to be read HERE
static const char FILENAME[] = "commands.txt";

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
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
void rwlock_init(rwlock_t* lock);

rwlock_t mutex;
hashRecord* head = NULL;
int lock_acquisitions = 0;
int lock_releases = 0;

int main() {
  // Open File
    FILE* fp;
    fp = fopen(FILENAME, "r");

    if (fp == NULL) {
        printf("Error: File \"%s\" not found.", FILENAME);
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

    pthread_t threads[numOfThreads];

    // Loop each line of the file
    for (int i = 0; i < numOfThreads; i++)
    {
        fscanf(fp, "%[^,],%[^,],%d", command, name, &salary);
        // EVERYTHING SHOULD BE PROCESSED HERE   
    }

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

    //acquire the writer-lock that protects the list and searches the linked list for the hash
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
    rwlock_release_writelock(&mutex);
}

//deletes key-value pair node if it exists
void delete(char* key_name) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, strlen(key_name));

    //acquire the writer-lock

    //search LL for the key

    //if key is found, remove node and free memory; otherwise, do nothing and return

    //release writer-lock and return
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

    //if found, return value; otherwise, return null
    rwlock_release_readlock(&mutex);
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
