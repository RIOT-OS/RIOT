#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <mutex.h>

typedef struct sem_t {
    int8_t value;
    int8_t locked;
    mutex_t mutex;
} sem_t;

void sem_init(sem_t *sem, int8_t value);
int sem_wait(sem_t *sem);
int sem_signal(sem_t *sem);
 
#endif /* SEMAPHORE_H*/
