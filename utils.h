#ifndef UTILS_H
#define UTILS_H

#include "hash_table.h"

long long current_timestamp();
hashRecord* linkedList_to_sorted_array(hashRecord* head);
void insertion_sort(hashRecord* array, int length);
int getLength(hashRecord* head);

#endif // UTILS_H
