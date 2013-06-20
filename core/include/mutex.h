/**
 * @defgroup mutex Mutexes / Synchronization
 * @ingroup kernel
 * @{
 */

/**
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef _MUTEX_H
#define _MUTEX_H

#include "queue.h"
#include "tcb.h"

/**
 * @brief Mutex structure. Should never be modified by the user.
 */
typedef struct mutex_t {
    /* fields are managed by mutex functions, don't touch */
    unsigned int val;       // @internal
    queue_node_t queue;     // @internal
} mutex_t;

/**
 * @brief Initializes a mutex object
 * @param mutex pre-allocated mutex structure.
 * @return Always returns 1, always succeeds.
 */
int mutex_init(struct mutex_t *mutex);


/**
 * @brief Tries to get a mutex, non-blocking.
 *
 * @param mutex Mutex-Object to lock. Has to be initialized first.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int mutex_trylock(struct mutex_t *mutex);

/**
 * @brief Tries to get a mutex, blocking.
 *
 * @param mutex Mutex-Object to lock. Has to be initialized first.
 *
 * @return 1 getting the mutex was successful
 * @return <1 there was an error.
 */
int mutex_lock(struct mutex_t *mutex);

/**
 * @brief Unlocks the mutex.
 *
 * @param mutex Mutex-Object to unlock.
 *
 * @param yield If yield==MUTEX_YIELD, force context-switch after waking up other waiter.
 */
void mutex_unlock(struct mutex_t *mutex, int yield);

#define MUTEX_YIELD 1
#define MUTEX_INISR 2

/**********************
 * internal functions *
 **********************/

void mutex_wake_waiters(struct mutex_t *mutex, int yield);
void mutex_wait(struct mutex_t *mutex);

/*struct mutex_entry_t * mutex_create_entry(int prio, struct tcb *proc);*/

/** @} */
#endif /* _MUTEX_H */

