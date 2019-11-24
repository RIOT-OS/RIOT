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
 */

#ifndef PTHREAD_TLS_H
#define PTHREAD_TLS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal representation of a thread-specific key.
 * @internal
 */
struct __pthread_tls_key;

/**
 * @brief   A single thread-specific datum.
 * @internal
 */
struct __pthread_tls_datum;

/**
 * @brief   A thread-specific key.
 */
typedef struct __pthread_tls_key *pthread_key_t;

/**
 * @brief Returns the requested tls
 * @param[in] key the identifier for the requested tls
 * @return returns pointer to the storage on success, a 0 value otherwise
 */
void *pthread_getspecific(pthread_key_t key);

/**
 * @brief Set and binds a specific tls to a key
 * @param[in] key the identifier for the tls
 * @param[in] value pointer to the location of the tls
 * @return returns 0 on success, an errorcode otherwise
 */
int pthread_setspecific(pthread_key_t key, const void *value);

/**
 * @brief Creates a new key to be used to identify a specific tls
 * @param[out] key the created key is scribed to the given pointer
 * @param[in] destructor function pointer called when non NULL just before the pthread exits
 * @return returns 0 on success, an errorcode otherwise
 */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));

/**
 * @brief Deletes a pthread_key_t that was previously created with pthread_key_create.
 * @details does not call the destructor of the key
 * @param[in] key the identifier of the key to be deleted
 * @return returns 0 on success, an errorcode otherwise
 */
int pthread_key_delete(pthread_key_t key);

/**
 * @brief Destroys all thread-specific keys for pthread `self_id`.
 * @param[in] self_id the result of pthread_self().
 * @internal
 */
void __pthread_keys_exit(int self_id);

/**
 * @brief Returns the pointer to the head of the list of thread-specific data.
 * @internal
 */
struct __pthread_tls_datum **__pthread_get_tls_head(int self_id) PURE;

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_TLS_H */

/**
 * @}
 */
