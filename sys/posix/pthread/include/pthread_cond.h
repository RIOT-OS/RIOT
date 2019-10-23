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

#ifndef PTHREAD_COND_H
#define PTHREAD_COND_H

#include <time.h>
#include "mutex.h"
#include "priority_queue.h"

#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
#   include "msp430_types.h"
#endif

#if defined(__WITH_AVRLIBC__)
/* avr-libc 'time.h' does not include 'sys/types.h' but we need 'clockid_t' */
#   include <sys/types.h>
#endif

#ifdef __MACH__
/* needed for AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER */
#include <AvailabilityMacros.h>
#if !defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
typedef int clockid_t;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note condition attributes are currently NOT USED in RIOT condition variables
 */
typedef struct {
    /** dumdidum */
    int __dummy;
} pthread_condattr_t;

/**
 * @brief Condition variable
 *
 * @warning fields are managed by cv functions, don't touch
 */
typedef struct {
    priority_queue_t queue; /**< Threads currently waiting to be signaled. */
} pthread_cond_t;

/**
 * @brief Initializes a condition attribute variable object using default values
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_condattr_init(pthread_condattr_t *attr);

/**
 * @brief Uninitializes a condition attribute variable object
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_condattr_destroy(pthread_condattr_t *attr);

/**
 * @brief Get the process-shared attribute in an initialized attributes object referenced by attr
 * @note NOT USED since RIOT is a single process OS
 * @param[in] attr pre-allocated condition attribute variable structure.
 * @param[out] pshared the pre-allocated process-shared variable.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);

/**
 * @brief Set the process-shared attribute in an initialized attributes object referenced by attr
 * @note NOT USED since RIOT is a single process OS
 * @param[in, out] attr pre-allocated condition attribute variable structure.
 * @param[in] pshared pshared the pre-allocated process-shared variable.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

/**
 * @brief Get the clock selected for the condition variable attribute attr.
 * @note currently NOT USED in RIOT.
 * @param[in] attr pre-allocated condition attribute variable structure.
 * @param[out] clock_id the clock ID that is used to measure the timeout service
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);

/**
 * @brief Set the clock selected for the condition variable attribute ATTR.
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
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);

/**
 * @brief Destroy the condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_destroy(pthread_cond_t *cond);

/**
 * @brief blocks the calling thread until the specified condition cond is signalled
 * @param[in, out] cond pre-allocated condition variable structure.
 * @param[in, out] mutex pre-allocated mutex variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_wait(pthread_cond_t *cond, mutex_t *mutex);

/**
 * @brief blocks the calling thread until the specified condition cond is signalled
 * @param[in, out] cond pre-allocated condition variable structure.
 * @param[in, out] mutex pre-allocated mutex variable structure.
 * @param[in] abstime pre-allocated timeout.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_timedwait(pthread_cond_t *cond, mutex_t *mutex, const struct timespec *abstime);

/**
 * @brief unblock at least one of the threads that are blocked on the specified condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_signal(pthread_cond_t *cond);

/**
 * @brief unblock all threads that are currently blocked on the specified condition variable cond
 * @param[in, out] cond pre-allocated condition variable structure.
 * @return returns 0 on success, an errorcode otherwise.
 */
int pthread_cond_broadcast(pthread_cond_t *cond);

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_COND_H */

/**
 * @}
 */
