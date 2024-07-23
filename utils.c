#include "utils.h"
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long microseconds = (te.tv_sec * 1000000) + te.tv_usec;

    return microseconds;
}

hashRecord* linkedList_to_sorted_array(hashRecord* head) {
    int length = getLength(head);

    hashRecord* array = (hashRecord*)malloc(length * sizeof(hashRecord));

    hashRecord* current = head;

    for (int i = 0; i < length; i++) {
        array[i] = *current;
        current = current->next;
    }

    insertion_sort(array, length);

    return array;
}

void insertion_sort(hashRecord* array, int length) {
    for (int i = 1; i < length; i++) {
        hashRecord key = array[i];
        int j = i - 1;

        while (j >= 0 && array[j].hash > key.hash) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
}

int getLength(hashRecord* head) {
    int length = 0;

    hashRecord* current = head;
    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}