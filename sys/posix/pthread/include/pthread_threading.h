/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Thread creation features.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifndef __SYS__POSIX__PTHREAD_THREADING__H
#define __SYS__POSIX__PTHREAD_THREADING__H

#include "attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief        Datatype to identify a POSIX thread.
 * @note         The pthread ids are one off to the index in the internal array.
 */
typedef unsigned pthread_t;

/**
 * @brief        Spawn a new POSIX thread.
 * @details      This functions starts a new thread.
 *               The thread will be joinable (from another pthread),
 *               unless `attr` tells to create the thread detached.
 *               A non-detached thread must be joined will stay a zombie into it is joined.
 *               You can call pthread_exit() inside the thread, or return from `start_routine()`.
 * @note         Cancellation is currently not implemented.
 * @note         In an embedded system you probably want to supply a statically allocated stack in `attr`.
 * @param[out]   newthread       The identifier of the new thread.
 * @param[in]    attr            An attribute set that describes how the new thread should be started.
 * @param[in]    start_routine   The entry point of the new thread.
 * @param[in]    arg             Argument supplied to `start_routine`.
 * @return       `== 0` on success.
 *               `!= 0` on error.
 */
int pthread_create(pthread_t *newthread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);

/**
 * @brief        Exit calling pthread.
 * @note         Only pthreads must call this function.
 *               Native threads must call sched_thread_exit().
 *               A pthread must not call sched_thread_exit().
 * @param[out]   retval   Return value, supplied to a joining thread.
 * @return       This function does not return.
 */
void pthread_exit(void *retval) NORETURN;

/**
 * @brief        Join a pthread.
 * @details      The current thread sleeps until `th` exits.
 *               The exit value of `th` gets written into `thread_return`.
 *               You can only join pthreads, and only pthreads can join.
 *               A thread must not join itself.
 * @param[in]    th              pthread to join, the id was supplied by pthread_create()
 * @param[out]   thread_return   Exit code of `th`.
 * @return       `== 0` on success.
 *               `!= 0` on error.
 */
int pthread_join(pthread_t th, void **thread_return);

/**
 * @brief        Make a pthread unjoinable.
 * @details      The resources of a detached thread get released as soon as it exits,
 *               without the need to call pthread_join() out of another pthread.
 *               In fact you cannot join a detached thread, it will return an error.
 *               Detaching a thread while another thread tries to join it causes undefined behavior.
 *               A pthread may detach himself.
 *               A non-pthread may call this function, too.
 *               A pthread cannot be "attached" again.
 * @param[in]    th   pthread to detach.
 * @return       `== 0` on success.
 *               `!= 0` on error.
 */
int pthread_detach(pthread_t th);

/**
 * @brief        Returns the pthread id of the calling/current thread.
 * @note         This function should not be used to determine if the calling thread is a pthread.
 *               If your logic is sane then there should be no need to do that.
 * @return       `> 0` identifies the calling pthread.
 *               `== 0` if the calling thread is not a pthread.
 */
pthread_t pthread_self(void);

/**
 * @brief        Compared two pthread identifiers.
 * @return       `0` if the ids identify two different threads.
 */
static inline int pthread_equal(pthread_t thread1, pthread_t thread2)
{
    return thread1 == thread2;
}

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
