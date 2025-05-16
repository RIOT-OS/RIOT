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
 * @brief   Cleanup primitives for pthread threads.
 * @note    Do not include this header file directly, but pthread.h.
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal structure for pthread_cleanup_push()
 */
typedef struct __pthread_cleanup_datum
{
    /** Cleanup handler to call next. */
    struct __pthread_cleanup_datum *__next;

    /** Cleanup routine to call. */
    void (*__routine)(void *arg);

    /** Argument to supply. */
    void *__arg;
} __pthread_cleanup_datum_t;

/**
 * @brief       Internal function to be called by pthread_cleanup_push()
 * @details     The previous top of the stack gets stored in `datum->next`.
 * @param[in]   datum   Allocated in the stack, datum tells how to cleanup.
 */
void __pthread_cleanup_push(__pthread_cleanup_datum_t *datum);

/**
 * @brief       Internal function to be called by pthread_cleanup_push()
 * @details     This function leaves the cleanup frame that was opened with __pthread_cleanup_push().
 * @param[in]   datum     Parameter that was supplied to __pthread_cleanup_push().
 * @param[in]   execute   Iff `!= 0` call cleanup handler.
 */
void __pthread_cleanup_pop(__pthread_cleanup_datum_t *datum, int execute);

/*
 * Notice: `pthread_cleanup_*` has to be defined as a macro, because the cleanup
 * stack needs extra data. The standard explicitly allows defining these
 * functions as macros.   The alternative would be malloc.
 */

/**
 * @brief       Open a cleanup frame.
 * @details     This function must be paired with pthread_cleanup_pop().
 * @details     The cleanup function is called if the current thread exists
 *              inside the frame by means of pthread_exit(), or if the thread
 *              was cancelled.
 * @details     You must not `return`, `continue`, or `break` out of the cleanup frame.
 * @details     The frame opens a scope. Variables declared inside this scope
 *              won't be visible outside.
 * @param[in]   ROUTINE   Function to call on cleanup.
 * @param[in]   ARG       Argument to supply to the cleanup handler.
 */
#define pthread_cleanup_push(ROUTINE, ARG) \
    do { \
        __extension__ __pthread_cleanup_datum_t ____datum__ = { \
            .__routine = (ROUTINE), \
            .__arg = (ARG), \
        }; \
        __extension__ int ____execute__ = 1; \
        __pthread_cleanup_push(&____datum__); \
        do { \
            do { } while (0)

/**
 * @brief       Closes a cleanup frame
 * @details     Must be paired with pthread_cleanup_push().
 * @param[in]   EXECUTE   Iff `!= 0` call cleanup handler.
 */
#define pthread_cleanup_pop(EXECUTE) \
            ____execute__ = (EXECUTE); \
        } while (0); \
        __pthread_cleanup_pop(&____datum__, ____execute__); \
    } while (0)

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
