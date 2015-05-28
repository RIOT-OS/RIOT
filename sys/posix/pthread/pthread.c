/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup pthread POSIX threads
 * POSIX conforming multi-threading features.
 * @ingroup posix
 * @{
 * @file
 * @brief   Thread creation features.
 * @see     [The Open Group Base Specifications Issue 7: pthread.h - threads](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cpu_conf.h"
#include "irq.h"
#include "kernel_internal.h"
#include "msg.h"
#include "mutex.h"
#include "priority_queue.h"
#include "thread.h"
#include "sched.h"

#include "pthread.h"

#define ENABLE_DEBUG (0)

#if ENABLE_DEBUG
#   define PTHREAD_REAPER_STACKSIZE THREAD_STACKSIZE_MAIN
#   define PTHREAD_STACKSIZE THREAD_STACKSIZE_MAIN
#else
#   define PTHREAD_REAPER_STACKSIZE THREAD_STACKSIZE_DEFAULT
#   define PTHREAD_STACKSIZE THREAD_STACKSIZE_DEFAULT
#endif

#include "debug.h"

enum pthread_thread_status {
    PTS_RUNNING,
    PTS_DETACHED,
    PTS_ZOMBIE,
};

typedef struct pthread_thread {
    kernel_pid_t thread_pid;

    enum pthread_thread_status status;
    kernel_pid_t joining_thread;
    void *returnval;
    bool should_cancel;

    void *(*start_routine)(void *);
    void *arg;

    char *stack;

    struct __pthread_tls_datum *tls_head;

    __pthread_cleanup_datum_t *cleanup_top;
} pthread_thread_t;

static pthread_thread_t *volatile pthread_sched_threads[MAXTHREADS];
static struct mutex_t pthread_mutex;

static volatile kernel_pid_t pthread_reaper_pid = KERNEL_PID_UNDEF;

static char pthread_reaper_stack[PTHREAD_REAPER_STACKSIZE];

static void *pthread_start_routine(void *pt_)
{
    pthread_thread_t *pt = pt_;
    void *retval = pt->start_routine(pt->arg);
    pthread_exit(retval);
}

static int insert(pthread_thread_t *pt)
{
    int result = -1;
    mutex_lock(&pthread_mutex);

    for (int i = 0; i < MAXTHREADS; i++){
        if (!pthread_sched_threads[i]) {
            pthread_sched_threads[i] = pt;
            result = i+1;
            break;
        }
    }

    mutex_unlock(&pthread_mutex);
    return result;
}

static void *pthread_reaper(void *arg)
{
    (void) arg;

    while (1) {
        msg_t m;
        msg_receive(&m);
        DEBUG("pthread_reaper(): free(%p)\n", m.content.ptr);
        free(m.content.ptr);
    }

    return NULL;
}

int pthread_create(pthread_t *newthread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    pthread_thread_t *pt = calloc(1, sizeof(pthread_thread_t));

    kernel_pid_t pthread_pid = insert(pt);
    if (pthread_pid == KERNEL_PID_UNDEF) {
        free(pt);
        return -1;
    }
    *newthread = pthread_pid;

    pt->status = attr && attr->detached ? PTS_DETACHED : PTS_RUNNING;
    pt->start_routine = start_routine;
    pt->arg = arg;

    bool autofree = attr == NULL || attr->ss_sp == NULL || attr->ss_size == 0;
    size_t stack_size = attr && attr->ss_size > 0 ? attr->ss_size : PTHREAD_STACKSIZE;
    void *stack = autofree ? malloc(stack_size) : attr->ss_sp;
    pt->stack = autofree ? stack : NULL;

    if (autofree && pthread_reaper_pid != KERNEL_PID_UNDEF) {
        mutex_lock(&pthread_mutex);
        if (pthread_reaper_pid != KERNEL_PID_UNDEF) {
            /* volatile pid to overcome problems with double checking */
            volatile kernel_pid_t pid = thread_create(pthread_reaper_stack,
                                             PTHREAD_REAPER_STACKSIZE,
                                             0,
                                             CREATE_STACKTEST,
                                             pthread_reaper,
                                             NULL,
                                             "pthread-reaper");
            pthread_reaper_pid = pid;
        }
        mutex_unlock(&pthread_mutex);
    }

    pt->thread_pid = thread_create(stack,
                                   stack_size,
                                   THREAD_PRIORITY_MAIN,
                                   CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                   pthread_start_routine,
                                   pt,
                                   "pthread");
    if (pt->thread_pid == KERNEL_PID_UNDEF) {
        free(pt->stack);
        free(pt);
        pthread_sched_threads[pthread_pid-1] = NULL;
        return -1;
    }

    sched_switch(THREAD_PRIORITY_MAIN);

    return 0;
}

void pthread_exit(void *retval)
{
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
    }
    else {
        pthread_thread_t *self = pthread_sched_threads[self_id - 1];

        while (self->cleanup_top) {
            __pthread_cleanup_datum_t *ct = self->cleanup_top;
            self->cleanup_top = ct->__next;

            ct->__routine(ct->__arg);
        }

        /* Prevent linking in pthread_tls.o if no TSS functions were used. */
        extern void __pthread_keys_exit(int self_id) __attribute__((weak));
        if (__pthread_keys_exit) {
            __pthread_keys_exit(self_id);
        }

        self->thread_pid = KERNEL_PID_UNDEF;
        DEBUG("pthread_exit(%p), self == %p\n", retval, (void *) self);
        if (self->status != PTS_DETACHED) {
            self->returnval = retval;
            self->status = PTS_ZOMBIE;

            if (self->joining_thread) {
                /* our thread got an other thread waiting for us */
                thread_wakeup(self->joining_thread);
            }
        }

        dINT();
        if (self->stack) {
            msg_t m;
            m.content.ptr = self->stack;
            msg_send_int(&m, pthread_reaper_pid);
        }
    }

    sched_task_exit();
}

int pthread_join(pthread_t th, void **thread_return)
{
    if (th < 1 || th > MAXTHREADS) {
        DEBUG("passed pthread_t th (%d) exceeds bounds of pthread_sched_threads[] in \"%s\"!\n", th, __func__);
        return -3;
    }

    pthread_thread_t *other = pthread_sched_threads[th-1];
    if (!other) {
        return -1;
    }

    switch (other->status) {
        case (PTS_RUNNING):
            other->joining_thread = sched_active_pid;
            /* go blocked, I'm waking up if other thread exits */
            thread_sleep();
            /* no break */
        case (PTS_ZOMBIE):
            if (thread_return) {
                *thread_return = other->returnval;
            }
            free(other);
            /* we only need to free the pthread layer struct,
            native thread stack is freed by other */
            pthread_sched_threads[th-1] = NULL;
            return 0;
        case (PTS_DETACHED):
            return -1;
    }

    return -2;
}

int pthread_detach(pthread_t th)
{
    if (th < 1 || th > MAXTHREADS) {
        DEBUG("passed pthread_t th (%d) exceeds bounds of pthread_sched_threads[] in \"%s\"!\n", th, __func__);
        return -2;
    }

    pthread_thread_t *other = pthread_sched_threads[th-1];
    if (!other) {
        return -1;
    }

    if (other->status == PTS_ZOMBIE) {
        free(other);
        /* we only need to free the pthread layer struct,
        native thread stack is freed by other */
        pthread_sched_threads[th-1] = NULL;
    } else {
        other->status = PTS_DETACHED;
    }

    return 0;
}

pthread_t pthread_self(void)
{
    pthread_t result = 0;
    mutex_lock(&pthread_mutex);
    kernel_pid_t pid = sched_active_pid; /* sched_active_pid is volatile */
    for (int i = 0; i < MAXTHREADS; i++) {
        if (pthread_sched_threads[i] && pthread_sched_threads[i]->thread_pid == pid) {
            result = i+1;
            break;
        }
    }
    mutex_unlock(&pthread_mutex);
    return result;
}

int pthread_cancel(pthread_t th)
{
    pthread_thread_t *other = pthread_sched_threads[th-1];
    if (!other) {
        return -1;
    }

    other->should_cancel = 1;

    return 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    (void) state;
    (void) oldstate;
    return -1;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    (void) type;
    (void) oldtype;
    return -1;
}

void pthread_testcancel(void)
{
    pthread_t self = pthread_self();

    if (self == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    if (pthread_sched_threads[self-1]->should_cancel) {
        pthread_exit(PTHREAD_CANCELED);
    }
}

void __pthread_cleanup_push(__pthread_cleanup_datum_t *datum)
{
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    pthread_thread_t *self = pthread_sched_threads[self_id-1];
    datum->__next = self->cleanup_top;
    self->cleanup_top = datum;
}

void __pthread_cleanup_pop(__pthread_cleanup_datum_t *datum, int execute)
{
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    pthread_thread_t *self = pthread_sched_threads[self_id-1];
    self->cleanup_top = datum->__next;

    if (execute != 0) {
        /* "The pthread_cleanup_pop() function shall remove the routine at the
         *  top of the calling thread's cancellation cleanup stack and optionally
         *  invoke it (if execute is non-zero)." */
        datum->__routine(datum->__arg);
    }
}

struct __pthread_tls_datum **__pthread_get_tls_head(int self_id)
{
    pthread_thread_t *self = pthread_sched_threads[self_id-1];
    return self ? &self->tls_head : NULL;
}
