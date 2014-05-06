#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H	1

#include <time.h>

/** Value returned if `sem_open' failed.  */
#define SEM_FAILED      ((sem_t *) 0)

#include "queue.h"

typedef struct sem {
    volatile unsigned int value;
    queue_t queue;
} sem_t;

/**
 * @brief Initialize semaphore object SEM to VALUE.
 *
 * @param sem Semaphore to initialize
 * @param pshared unused
 * @param value Value to set
 */
int sem_init(sem_t *sem, int pshared, unsigned int value);

/**
 * @brief Free resources associated with semaphore object SEM.
 *
 * @param sem Semaphore to destroy
 */
int sem_destroy(sem_t *sem);

/*
 * @brief Open a named semaphore NAME with open flags OFLAG.
 *
 * @brief WARNING: named semaphore are currently not supported
 *
 * @param name Name to set
 * @param oflag Flags to set
 */
sem_t *sem_open(const char *name, int oflag, ...);

/**
 * @brief Close descriptor for named semaphore SEM.
 *
 * @brief WARNING: named semaphore are currently not supported
 *
 * @param sem Semaphore to close
 */
int sem_close(sem_t *sem);

/**
 * @brief Remove named semaphore NAME.
 *
 * @brief WARNING: named semaphore are currently not supported
 *
 * @param name Name to unlink
 */
int sem_unlink(const char *name);

/**
 * @brief Wait for SEM being posted.
 *
 * @param sem Semaphore to wait
 */
int sem_wait(sem_t *sem);

/**
 * @brief Similar to `sem_wait' but wait only until ABSTIME.
 *
 * @brief WARNING: currently not supported
 *
 * @param sem Semaphore to wait on
 * @param abstime Max time to wait for a post
 *
 */
int sem_timedwait(sem_t *sem, const struct timespec *abstime);

/**
 * @brief Test whether SEM is posted.
 *
 * @param sem Semaphore to trywait on
 *
 */
int sem_trywait(sem_t *sem);

/**
 * @brief Post SEM.
 *
 * @param sem Semaphore to post on
 */
int sem_post(sem_t *sem);

/**
 * @brief Get current value of SEM and store it in *SVAL.
 *
 * @param sem Semaphore to get value from
 * @param sval place whre value goes to
 */
int sem_getvalue(sem_t *sem, int *sval);

#endif	/* semaphore.h */
