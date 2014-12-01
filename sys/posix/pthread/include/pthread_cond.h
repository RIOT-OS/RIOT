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
 * @brief       RIOT POSIX condition variable API
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#ifndef __SYS__POSIX__PTHREAD_COND__H
#define __SYS__POSIX__PTHREAD_COND__H

#include <time.h>
#include "mutex.h"

#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
#   include "msp430_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note condition attributes are currently NOT USED in RIOT condition variables
 */
typedef struct pthread_condattr_t {
    int __dummy;
} pthread_condattr_t;

typedef struct pthread_cond_t {
    /* fields are managed by cv functions, don't touch */
    priority_queue_t queue; /**< Threads currently waiting to be signaled. */
} pthread_cond_t;

/**
 * @brief Initializes a condition attribute variable object using default values
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_condattr_init(struct pthread_condattr_t *attr);

/**
 * @brief Uninitializes a condition attribute variable object
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_condattr_destroy(struct pthread_condattr_t *attr);

/**
 * @brief Get the process-shared attribute in an initialised attributes object referenced by attr
 * @note NOT USED since RIOT is a single process OS
 * @param[in] attr pre-allocated condition attribute variable structure.
 * @param[out] pshared the pre-allocated process-shared variable.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);

/**
 * @brief Set the process-shared attribute in an initialised attributes object referenced by attr
 * @note NOT USED since RIOT is a single process OS
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @param[in] pshared pshared the pre-allocated process-shared variable.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

/**
 * @brief Get the clock selected for the conditon variable attribute attr.
 * @note currently NOT USED in RIOT.
 * @param[in] attr pre-allocated condition attribute variable structure.
 * @param[out] clock_id the clock ID that is used to measure the timeout service
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);

/**
 * @brief Set the clock selected for the conditon variable attribute ATTR.
 * @note currently NOT USED in RIOT.
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @param[in] clock_id the clock ID that shall be used to measure the timeout service
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);

/**
 * @brief Initializes a condition variable object
 * @param[in, out] cond pre-allocated condition variable structure.
 * @param[in] attr pre-allocated condition attribute variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_init(struct pthread_cond_t *cond, struct pthread_condattr_t *attr);

/**
 * @brief Destroy the condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_destroy(struct pthread_cond_t *cond);

/**
 * @brief blocks the calling thread until the specified condition cond is signalled
 * @param[in, out] cond pre-allocated condition variable structure.
 * @param[in, out] mutex pre-allocated mutex variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_wait(struct pthread_cond_t *cond, struct mutex_t *mutex);

/**
 * @brief blocks the calling thread until the specified condition cond is signalled
 * @param[in, out] cond pre-allocated condition variable structure.
 * @param[in, out] mutex pre-allocated mutex variable structure.
 * @param[in] abstime pre-allocated timeout.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_timedwait(struct pthread_cond_t *cond, struct mutex_t *mutex, const struct timespec *abstime);

/**
 * @brief unblock at least one of the threads that are blocked on the specified condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_signal(struct pthread_cond_t *cond);

/**
 * @brief unblock all threads that are currently blocked on the specified condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_broadcast(struct pthread_cond_t *cond);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _CONDITION_VARIABLE_H */
