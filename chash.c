#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
void insert(char* key_name, uint32_t salary);
void delete(char* key_name);
uint32_t search(char* key_name);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
void rwlock_init(rwlock_t* lock);

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

rwlock_t mutex;
hashRecord* head = NULL;

int main() {

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
void insert(char key_name[], uint32_t salary) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

    //acquire the writer-lock that protects the list and searches the linked list for the hash
    rwlock_acquire_writelock(&mutex);

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
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

    //acquire the writer-lock

    //search LL for the key

    //if key is found, remove node and free memory; otherwise, do nothing and return

    //release writer-lock and return
}

//searches for key-value pair node and if found, returns the value; if not found, returns NULL
//if found, the caller prints the record; otherwise, the caller prints "No Record Found"
uint32_t search(char* key_name) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

    // acquire reader-lock

    //search LL for the key

    //if found, return value; otherwise, return null
}

void rwlock_init(rwlock_t* lock) {
    lock->readers = 0;
    Sem_init(&lock->lock, 1);
    Sem_init(&lock->writelock, 1);
}

void rwlock_acquire_readlock(rwlock_t* lock) {
    Sem_wait(&lock->lock);
    lock->readers++;
    if (lock->readers == 1)
        Sem_wait(&lock->writelock);
    Sem_post(&lock->lock);
}

void rwlock_release_readlock(rwlock_t* lock) {
    Sem_wait(&lock->lock);
    lock->readers--;
    if (lock->readers == 0)
        Sem_post(&lock->writelock);
    Sem_post(&lock->lock);
}

void rwlock_acquire_writelock(rwlock_t* lock) {
    Sem_wait(&lock->writelock);
}

void rwlock_release_writelock(rwlock_t* lock) {
    Sem_post(&lock->writelock);
}