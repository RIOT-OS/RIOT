/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief       RIOT POSIX thread local storage
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "pthread.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct __pthread_tls_datum {
    pthread_key_t key;
    struct __pthread_tls_datum *next;
    void *value;
} tls_data_t;

struct __pthread_tls_key {
    void (*destructor)(void *);
};

/**
 * @brief   Used while manipulating the TLS of a pthread.
 */
static mutex_t tls_mutex;

/**
 * @brief        Find a thread-specific datum.
 * @param[in]    tls    Pointer to the list of the thread-specific datums.
 * @param[in]    key    The key to look up.
 * @param[out]   prev   The datum before the result. `NULL` if the result is the first key. Spurious if the key was not found.
 * @returns      The datum or `NULL`.
 */
static tls_data_t *find_specific(tls_data_t **tls, pthread_key_t key, tls_data_t **prev)
{
    tls_data_t *specific = *tls;
    *prev = NULL;

    while (specific) {
        if (specific->key == key) {
            return specific;
        }

        *prev = specific;
        specific = specific->next;
    }

    return 0;
}

/**
 * @brief       Find or allocate a thread specific datum.
 * @details     The `key` must be initialized.
 *              The result will be the head of the thread-specific datums afterwards.
 * @param[in]   key   The key to lookup.
 * @returns     The datum. `NULL` on ENOMEM or if the caller is not a pthread.
 */
static tls_data_t *get_specific(pthread_key_t key)
{
    pthread_t self_id = pthread_self();
    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return NULL;
    }

    tls_data_t **tls = __pthread_get_tls_head(self_id);
    tls_data_t *prev, *specific = find_specific(tls, key, &prev);

    /* Did the datum already exist? */
    if (specific) {
        if (prev) {
            /* Move the datum to the front for a faster next lookup. */
            /* Let's pretend that we have a totally degenerated splay tree. ;-) */
            prev->next = specific->next;
            specific->next = *tls;
            *tls = specific;
        }
        return specific;
    }

    /* Allocate new datum. */
    specific = malloc(sizeof (*specific));
    if (specific) {
        specific->key = key;
        specific->next = *tls;
        specific->value = NULL;
        *tls = specific;
    }
    else {
        DEBUG("ERROR out of memory in %s!\n", __func__);
    }
    return specific;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    *key = malloc(sizeof (**key));
    if (!*key) {
        return ENOMEM;
    }

    (*key)->destructor = destructor;
    return 0;
}

int pthread_key_delete(pthread_key_t key)
{
    if (!key) {
        return EINVAL;
    }

    mutex_lock(&tls_mutex);
    for (unsigned i = 1; i <= MAXTHREADS; ++i) {
        tls_data_t **tls = __pthread_get_tls_head(i);
        if (!tls) {
            continue;
        }

        tls_data_t *prev, *specific = find_specific(tls, key, &prev);
        if (specific) {
            if (prev) {
                prev->next = specific->next;
            }
            else {
                *tls = specific->next;
            }
            free(specific);
        }
    }
    mutex_unlock(&tls_mutex);

    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    if (!key) {
        return NULL;
    }

    mutex_lock(&tls_mutex);
    tls_data_t *specific = get_specific(key);
    void *result = specific ? specific->value : NULL;
    mutex_unlock(&tls_mutex);

    return result;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    if (!key) {
        return EINVAL;
    }

    mutex_lock(&tls_mutex);
    tls_data_t *specific = get_specific(key);
    if (specific) {
        specific->value = (void *) value;
    }
    mutex_unlock(&tls_mutex);

    return specific ? 0 : ENOMEM;
}

void __pthread_keys_exit(int self_id)
{
    tls_data_t **tls = __pthread_get_tls_head(self_id);

    /* Calling the dtor could cause another pthread_exit(), so we dehead and free before calling it. */
    mutex_lock(&tls_mutex);
    for (tls_data_t *specific; (specific = *tls); ) {
        *tls = specific->next;
        void *value = specific->value;
        void (*destructor)(void *) = specific->key->destructor;
        free(specific);

        if (value && destructor) {
            mutex_unlock(&tls_mutex);
            destructor(value);
            mutex_lock(&tls_mutex);
        }
    }
    mutex_unlock(&tls_mutex);
}
