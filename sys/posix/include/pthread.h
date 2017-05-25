#ifndef RIOT_PTHREAD_H
#define RIOT_PTHREAD_H	1

#include <time.h>

#include "mutex.h"
#include "sched.h"

#include "pthreadtypes.h"

/* Create a new thread, starting with execution of START-ROUTINE
 getting passed ARG.  Creation attributed come from ATTR.  The new
 handle is stored in *NEWTHREAD.  */
int pthread_create(pthread_t *newthread, const pthread_attr_t *attr,
        void *(*start_routine)(void *), void *arg);

/* Terminate calling thread.

 The registered cleanup handlers are called via exception handling .*/
void pthread_exit(void *retval);

/* Make calling thread wait for termination of the thread TH.  The
 exit status of the thread is stored in *THREAD_RETURN, if THREAD_RETURN
 is not NULL.

 This function is a cancellation point and therefore not marked with
 .  */
int pthread_join(pthread_t th, void **thread_return);

/* Indicate that the thread TH is never to be joined with PTHREAD_JOIN.
 The resources of TH will therefore be freed immediately when it
 terminates, instead of waiting for another thread to perform PTHREAD_JOIN
 on it.  */
int pthread_detach(pthread_t th);

/* Obtain the identifier of the current thread.  */
pthread_t pthread_self(void);

/* Compare two thread identifiers.  */
int pthread_equal(pthread_t thread1, pthread_t thread2);

#include "pthread/pthread_attr.h"

/* Functions for scheduling control.  */

/* Set the scheduling parameters for TARGET_THREAD according to POLICY
 and *PARAM.  */
int pthread_setschedparam(pthread_t target_thread, int policy,
        const struct sched_param *param);

/* Return in *POLICY and *PARAM the scheduling parameters for TARGET_THREAD. */
int pthread_getschedparam(pthread_t target_thread, int * policy,
        struct sched_param * param);

/* Set the scheduling priority for TARGET_THREAD.  */
int pthread_setschedprio(pthread_t target_thread, int prio);

/* Functions for handling initialization.  */

/* Guarantee that the initialization function INIT_ROUTINE will be called
 only once, even if pthread_once is executed several times with the
 same ONCE_CONTROL argument. ONCE_CONTROL must point to a static or
 variable initialized to PTHREAD_ONCE_INIT.

 The initialization functions might throw exception */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/* Functions for handling cancellation.

 Note that these functions are explicitly not marked to not throw an
 exception in C++ code.  If cancellation is implemented by unwinding
 this is necessary to have the compiler generate the unwind information.  */

/* Set cancelability state of current thread to STATE, returning old
 state in *OLDSTATE if OLDSTATE is not NULL.  */
int pthread_setcancelstate(int state, int *oldstate);

/* Set cancellation state of current thread to TYPE, returning the old
 type in *OLDTYPE if OLDTYPE is not NULL.  */
int pthread_setcanceltype(int type, int *oldtype);

/* Cancel THREAD immediately or at the next possibility.  */
int pthread_cancel(pthread_t th);

/* Test for pending cancellation for the current thread and terminate
 the thread as per pthread_exit(PTHREAD_CANCELED) if it has been
 cancelled.  */
void pthread_testcancel(void);

#include "pthread/pthread_mutex.h"

#include "pthread/pthread_rwlock.h"

#include "pthread/pthread_spin.h"

#include "pthread/pthread_barrier.h"

#endif	/* pthread.h */
