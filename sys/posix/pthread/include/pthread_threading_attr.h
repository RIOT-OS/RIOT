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
 * @brief   Thread creation features (attributes).
 * @note    Do not include this header file directly, but pthread.h.
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     An attribute set to supply to pthread_create()
 * @details   A zeroed out datum is default initialized.
 * @see       pthread_create() for further information
 */
typedef struct
{
    uint8_t detached; /**< Start in detached state. */
    char *ss_sp; /**< Stack to use for new thread. */
    size_t ss_size; /**< Size of dynamically allocated stack, or supplied stack, resp. */
} pthread_attr_t;

/**
 * @brief   This structure is unused right now, and only exists for POSIX compatibility.
 */
struct sched_param {
    /** Todo is the greates magician in the land of RIOT */
    int todo; /* TODO */
};

#define PTHREAD_CREATE_JOINABLE (0) /**< Create new pthread as joinable (default). */
#define PTHREAD_CREATE_DETACHED (1) /**< Create new pthread in detached state. */

/**
 * @brief           Initialize attributes for a new pthread.
 * @see             pthread_create()
 * @details         A zeroed out `attr` is initialized.
 * @param[in,out]   attr   Structure to initialize
 * @returns         0, invocation cannot fail
 */
int pthread_attr_init(pthread_attr_t *attr);

/**
 * @brief           Destroys an attribute set.
 * @details         Since pthread_attr_t does not hold dynamic data, this is a no-op.
 * @returns         0, since this a no-op that cannot fail
 */
int pthread_attr_destroy(pthread_attr_t *attr);

/**
 * @brief           Tells whether to create a new pthread in a detached state.
 * @param[in]       attr          Attribute set to query.
 * @param[out]      detachstate   Either PTHREAD_CREATE_JOINABLE or PTHREAD_CREATE_DETACHED.
 * @returns         0, the invocation cannot fail
 */
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

/**
 * @brief           Sets whether to create a new pthread in a detached state.
 * @note            Supplying a value different form PTHREAD_CREATE_JOINABLE or PTHREAD_CREATE_DETACHED
 *                  causes undefined behavior.
 * @param[in,out]   attr          Attribute set to operate on.
 * @param[in]       detachstate   Either PTHREAD_CREATE_JOINABLE or PTHREAD_CREATE_DETACHED.
 * @returns         `0` on success.
 *                  `-1` if you managed to supply an illegal value in `detachstate`.
 */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in]       attr        Unused
 * @param[out]      guardsize   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in,out]   attr        Unused
 * @param[in]       guardsize   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in]       attr    Unused
 * @param[out]      param   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in,out]   attr    Unused
 * @param[in]       param   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in]       attr     Unused
 * @param[out]      policy   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in,out]   attr     Unused
 * @param[in]       policy   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in]       attr      Unused
 * @param[out]      inherit   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in,out]   attr      Unused
 * @param[in]       inherit   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in]       attr    Unused
 * @param[out]      scope   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);

/**
 * @brief           This function is unused right now, and only exists for POSIX compatibility.
 * @param[in,out]   attr    Unused
 * @param[in]       scope   Unused
 * @returns         -1, this function fails
 */
int pthread_attr_setscope(pthread_attr_t *attr, int scope);

/**
 * @brief           Query assigned stack for new pthread.
 * @see             pthread_attr_setstackaddr() for more information
 * @param[in]       attr        Attribute set to query
 * @param[out]      stackaddr   Pointer to previously assigned stack, or `NULL` for dynamic allocation.
 * @returns         0, this invocation cannot fail
 */
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);

/**
 * @brief           Set address of the stack to use for the new pthread.
 * @details         If `*stackaddr == NULL`, then the stack is dynamically allocated with malloc().
 *                  No two running threads may operate on the same stack.
 *                  The stack of a zombie thread (i.e. a non-detached thread that exited but was not yet joined)
 *                  may in theory be reused even before joining, though there might be problems
 *                  if the stack was preempted before pthread_exit() completed.
 * @param[in,out]   attr        Attribute set to operate on.
 * @param[in]       stackaddr   Static stack to use, or `NULL` for dynamic allocation.
 * @returns         0, this invocation cannot fail
 */
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);

/**
 * @brief           Query set stacksize for new pthread.
 * @details         Returns default stack size of the value is yet unset.
 * @param[in]       attr        Attribute set to query.
 * @param[out]      stacksize   Assigned or default stack size, resp.
 * @returns         0, this invocation cannot fail
 */
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);

/**
 * @brief           Set the stack size for the new pthread.
 * @details         If you use pthread_attr_getstackaddr() to assign a static stack,
 *                  then you must use this function to set the size of the stack.
 *                  In case of dynamic memory allocation this overrules the default value.
 * @param[in,out]   attr        Attribute set to operate on
 * @param[in]       stacksize   Size of the stack of the new thread.
 *                              Supply `0` to use the default value.
 * @return          0, this invocation cannot fail.
 */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

/**
 * @brief           Query set stacksize for new pthread.
 * @param[in]       attr        Attribute set to query.
 * @param[out]      stackaddr   Pointer to previously assigned stack, or `NULL` for dynamic allocation.
 * @param[out]      stacksize   Assigned or default stack size, resp.
 * @returns         0, this invocation cannot fail
 */
int pthread_attr_getstack(pthread_attr_t *attr, void **stackaddr, size_t *stacksize);

/**
 * @brief           Set address and stack size of the stack to use for the new pthread.
 * @details         This function requires setting the address as well as the size
 *                  since only setting the address will make the implementation
 *                  on some architectures impossible.
 *                  If `*stackaddr == NULL`, then the stack is dynamically allocated with malloc().
 *                  No two running threads may operate on the same stack.
 *                  The stack of a zombie thread (i.e. a non-detached thread that exited but was not yet joined)
 *                  may in theory be reused even before joining, though there might be problems
 *                  if the stack was preempted before pthread_exit() completed.
 * @param[in,out]   attr        Attribute set to operate on.
 * @param[in]       stackaddr   Static stack to use, or `NULL` for dynamic allocation.
 * @param[in]       stacksize   Size of the stack of the new thread.
 *                              Supply `0` to use the default value.
 * @returns         0, this invocation cannot fail
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
