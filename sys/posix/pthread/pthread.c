/**
 * POSIX implementation of threading.
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup posix
 * @{
 * @file    pthread.c
 * @brief   Implementation of pthread.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cpu-conf.h"
#include "irq.h"
#include "kernel_internal.h"
#include "msg.h"
#include "mutex.h"
#include "queue.h"
#include "thread.h"
#include "sched.h"

#include "pthread.h"

#define ENABLE_DEBUG (0)

#if ENABLE_DEBUG
#   define PTHREAD_REAPER_STACKSIZE KERNEL_CONF_STACKSIZE_MAIN
#   define PTHREAD_STACKSIZE KERNEL_CONF_STACKSIZE_MAIN
#else
#   define PTHREAD_REAPER_STACKSIZE KERNEL_CONF_STACKSIZE_DEFAULT
#   define PTHREAD_STACKSIZE KERNEL_CONF_STACKSIZE_DEFAULT
#endif

#include "debug.h"

enum pthread_thread_status {
    PTS_RUNNING,
    PTS_DETACHED,
    PTS_ZOMBIE,
};

typedef struct pthread_thread {
    int thread_pid;

    enum pthread_thread_status status;
    int joining_thread;
    void *returnval;
    bool should_cancel;

    void *(*start_routine)(void *);
    void *arg;

    char *stack;
} pthread_thread_t;

static pthread_thread_t *volatile pthread_sched_threads[MAXTHREADS];
static struct mutex_t pthread_mutex;

static volatile int pthread_reaper_pid = -1;

static char pthread_reaper_stack[PTHREAD_REAPER_STACKSIZE];

static void pthread_start_routine(void)
{
    pthread_t self = pthread_self();
    pthread_thread_t *pt = pthread_sched_threads[self];
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
            result = i;
            break;
        }
    }
    mutex_unlock(&pthread_mutex);
    return result;
}

static void pthread_reaper(void)
{
    while (1) {
        msg_t m;
        msg_receive(&m);
        DEBUG("pthread_reaper(): free(%p)\n", m.content.ptr);
        free(m.content.ptr);
    }
}

int pthread_create(pthread_t *newthread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    pthread_thread_t *pt = calloc(1, sizeof(pthread_thread_t));

    int pthread_pid = insert(pt);
    if (pthread_pid < 0) {
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

    if (autofree && pthread_reaper_pid < 0) {
        mutex_lock(&pthread_mutex);
        if (pthread_reaper_pid < 0) {
            /* volatile pid to overcome problems with double checking */
            volatile int pid = thread_create(pthread_reaper_stack,
                                             PTHREAD_REAPER_STACKSIZE,
                                             0,
                                             CREATE_STACKTEST,
                                             pthread_reaper,
                                             "pthread-reaper");
            pthread_reaper_pid = pid;
        }
        mutex_unlock(&pthread_mutex);
    }

    pt->thread_pid = thread_create(stack,
                                   stack_size,
                                   PRIORITY_MAIN,
                                   CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                   pthread_start_routine,
                                   "pthread");
    if (pt->thread_pid < 0) {
        free(pt->stack);
        free(pt);
        pthread_sched_threads[pthread_pid] = NULL;
        return -1;
    }

    sched_switch(active_thread->priority, PRIORITY_MAIN);

    return 0;
}

void pthread_exit(void *retval)
{
    pthread_thread_t *self = pthread_sched_threads[pthread_self()];
    self->thread_pid = -1;
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
    sched_task_exit();
}

int pthread_join(pthread_t th, void **thread_return)
{
    pthread_thread_t *other = pthread_sched_threads[th];
    if (!other) {
        return -1;
    }

    switch (other->status) {
        case (PTS_RUNNING):
            other->joining_thread = thread_pid;
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
            pthread_sched_threads[th] = NULL;
            return 0;
        case (PTS_DETACHED):
            return -1;
    }

    return -2;
}

int pthread_detach(pthread_t th)
{
    pthread_thread_t *other = pthread_sched_threads[th];
    if (!other) {
        return -1;
    }

    if (other->status == PTS_ZOMBIE) {
        free(other);
        /* we only need to free the pthread layer struct,
        native thread stack is freed by other */
        pthread_sched_threads[th] = NULL;
    } else {
        other->status = PTS_DETACHED;
    }

    return 0;
}

pthread_t pthread_self(void)
{
    pthread_t result = -1;
    mutex_lock(&pthread_mutex);
    int pid = thread_pid; /* thread_pid is volatile */
    for (int i = 0; i < MAXTHREADS; i++) {
        if (pthread_sched_threads[i] && pthread_sched_threads[i]->thread_pid == pid) {
            result = i;
            break;
        }
    }
    mutex_unlock(&pthread_mutex);
    return result;
}

int pthread_equal(pthread_t thread1, pthread_t thread2)
{
    return (thread1 == thread2);
}

int pthread_cancel(pthread_t th)
{
    pthread_thread_t *other = pthread_sched_threads[th];
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
    return 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    (void) type;
    (void) oldtype;
    return 0;
}

void pthread_testcancel(void)
{
    pthread_t self = pthread_self();
    if (pthread_sched_threads[self]->should_cancel) {
        pthread_exit(NULL);
    }
}
