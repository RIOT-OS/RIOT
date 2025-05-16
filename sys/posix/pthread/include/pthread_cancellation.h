/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Thread cancellation features.
 * @note    None of these functions are implemented, yet.
 * @note    Do not include this header file directly, but pthread.h.
 */


#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_CANCEL_DISABLE 0
#define PTHREAD_CANCEL_ENABLE  1

#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

/**
 * @brief Thread exit status after a canceled thread has terminated.
 */
#define PTHREAD_CANCELED ((void *) -2)

/**
 * @brief        Cancellation point are not supported, yet.
 * @param[in]    state      Unused
 * @param[out]   oldstate   Unused
 * @returns      `-1`, this invocation fails
 */
int pthread_setcancelstate(int state, int *oldstate);

/**
 * @brief        Cancellation point are not supported, yet.
 * @param[in]    type      Unused
 * @param[out]   oldtype   Unused
 * @returns      `-1`, this invocation fails
 */
int pthread_setcanceltype(int type, int *oldtype);

/**
 * @brief        Tells a pthread that it should exit.
 * @note         Cancellation points are not supported, yet.
 * @details      A pthread `th` can call pthread_testcancel().
 *               If pthread_cancel(th) was called before, it will exit then.
 * @param[in]    th   Pthread to tell that it should cancel.
 * @returns      `-1`, this invocation fails
 */
int pthread_cancel(pthread_t th);

/**
 * @brief        Exit the current pthread if pthread_cancel() was called for this thread before.
 * @details      If pthread_cancel() called before, the current thread exits with with the code #PTHREAD_CANCELED.
 */
void pthread_testcancel(void);

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
