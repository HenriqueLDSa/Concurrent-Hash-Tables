#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <semaphore.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct* next;
} hashRecord;

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
hashRecord* search(char* key_name);
void insert(char* key_name, uint32_t salary);
void delete(char* key_name);
void print_table();

#endif // HASH_TABLE_H