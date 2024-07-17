#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct* next;
} hashRecord;

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

    //if found, update it; if not, add it to the LL

    //release writer-lock and return
}

//deletes key-value pair node if it exists
void delete(char key_name[]) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash(key_name, sizeof(key_name) - 1);

    //acquire the writer-lock

    //search LL for the key

    //if key is found, remove node and free memory; otherwise, do nothing

    //release writer-lock and return
}

//searches for key-value pair node and if found, returns the value; if not found, returns NULL
//if found, the caller prints the record; otherwise, the caller prints "No Record Found"
uint32_t search(char key_name[]) {
    //compute the hash value of the key
    uint32_t hash = jenkins_one_at_a_time_hash(key_name, sizeof(key_name) - 1);

    // acquire reader-lock

    //search LL for the key

    //if found, return value; otherwise, return null
}
