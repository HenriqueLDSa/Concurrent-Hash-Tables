#include "rwlock.h"
#include "utils.h"
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>

extern int lock_acquisitions;
extern int lock_releases;
extern FILE* out;

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