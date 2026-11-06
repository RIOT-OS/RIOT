/*
 * SPDX-FileCopyrightText: 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Singletons features / single-shot execution.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Datatype to supply to pthread_once().
 * @details         This data type must be compatible with the one defined
 *                  in newlib's `include/sys/_pthreadtypes.h`.
 */
typedef struct {
  int   is_initialized;  /**< initialized */
  int   init_executed;   /**< init function executed */
} pthread_once_t;

/**
 * @def             PTHREAD_ONCE_INIT
 * @brief           Initialization for pthread_once_t.
 * @details         pthread_once_t variables are declared as initialized, but
 *                  the init function is not yet executed.
 */
#define PTHREAD_ONCE_INIT  { 1, 0 }

/**
 * @brief           Helper function that ensures that `init_routine` is called at once.
 * @details         Calling pthread_once() changes `once_control`.
 *                  For the same `once_control` the supplied `init_routine` won't get called again,
 *                  unless `once_control` is reset to #PTHREAD_ONCE_INIT or zeroed out.
 * @param[in,out]   once_control   Flag to ensure that the `init_routine` is called only once.
 * @param[in]       init_routine   Function to call if `once_control` was not used, yet.
 * @returns         0, this invocation cannot fail.
 */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
