/**
 * Implementation of pthread thread local storage.
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup posix
 * @{
 * @file
 * @brief   Implementation of pthread thread local storage.
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 */

#ifndef __SYS__POSIX__PTHREAD_KEYS__H
#define __SYS__POSIX__PTHREAD_KEYS__H

#include "attributes.h"

typedef struct pthread_key
{
    unsigned index;

    void (*destructor)(void*);
    struct pthread_key *next;
} pthread_key_t[1];

/**
 * @brief Number of calls to pthread_key_create.
 */
extern int __pthread_key_count;

/**
 * Array of thread-local key values of the calling thread.
 */
void **__pthread_keys(void) PURE;

/**
 * @brief       Calls the dtors of the thread specific data.
 * @param[in]   keys_   Keys to clean up.
 *
 * Called during pthread_exit().
 */
void __pthread_keys_exit(void *keys_);

/**
 * @brief        Creates a key for the thread specific key-value store.
 * @param[out]   key   Key to allocate
 * @returns      0, cannot fail
 *
 * This function does not implemented full functionallity described in the POSIX standard!
 * The keys are not dynamic.
 * All calls to pthread_key_create() must be done before the first call to pthread_create().
 *
 * A good place to call this function is auto_init() and main().
 */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));

/**
 * @brief       This function fails.
 * @param[in]   key   The key you want to get a failure for.
 * @returns     -1, because this function fails
 *
 * Our pthread TLS implementation is not dynamic, see pthread_key_create().
 * You cannot delete keys. Once you defined a key, you are stuck with it.
 */
int pthread_key_delete(const pthread_key_t key);

/**
 * @brief       Gets the value of a thread-local key
 * @param[in]   KEY   The pthread_key_t key to return a value for
 * @return      The value associated with KEY
 *
 * This function is implemented as a macro to enable compiler optimizations.
 * __pthread_keys() is PURE, hence multiple calls to __pthread_keys() can be eliminated.
 */
#define pthread_getspecific(KEY) (__pthread_keys()[(KEY)[0].index])

/**
 * @brief       Sets the value of a thread-local key
 * @param[in]   KEY     The pthread_key_t key to set its value for
 * @param[in]   VALUE   Value to associate with KEY
 * @return      0, cannot fail
 *
 * This function is implemented as a macro to enable compiler optimizations.
 * __pthread_keys() is PURE, hence multiple calls to __pthread_keys() can be eliminated.
 */
#define pthread_setspecific(KEY, VALUE) \
        ((__pthread_keys()[(KEY)[0].index] = (void *) (VALUE)), 0)

#endif

/**
 * @}
 */
