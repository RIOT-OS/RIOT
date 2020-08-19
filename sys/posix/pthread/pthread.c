/*
 * Copyright (C) 2013 Freie Universität Berlin,
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Thread creation features.
 * @see     [The Open Group Base Specifications Issue 7: pthread.h - threads](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @author  Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 * @}
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "cpu_conf.h"
#include "irq.h"
#include "msg.h"
#include "mutex.h"
#include "thread.h"
#include "sched.h"

#include "pthread.h"

#ifdef HAVE_MALLOC_H
//#undef HAVE_MALLOC_H
#include <malloc.h>
#endif

#define ENABLE_DEBUG (0)

#ifndef CONFIG_PTHREAD_REAPER_BASE_STACKSIZE
#define CONFIG_PTHREAD_REAPER_BASE_STACKSIZE (THREAD_STACKSIZE_IDLE)
#endif

#if ENABLE_DEBUG
#   define PTHREAD_REAPER_STACKSIZE ((CONFIG_PTHREAD_REAPER_BASE_STACKSIZE) + \
                                     THREAD_EXTRA_STACKSIZE_PRINTF)
#   define PTHREAD_STACKSIZE THREAD_STACKSIZE_MAIN
#else
#   define PTHREAD_REAPER_STACKSIZE (CONFIG_PTHREAD_REAPER_BASE_STACKSIZE)
#   define PTHREAD_STACKSIZE THREAD_STACKSIZE_DEFAULT
#endif

#include "debug.h"

typedef enum {
    PTS_JOINABLE,
    PTS_DETACHED,
    PTS_ZOMBIE,
} pthread_thread_status_t;

typedef struct {
    kernel_pid_t thread_pid;

    pthread_thread_status_t status;
    mutex_t join_mutex;
    void *returnval;
    bool should_cancel;

    void *(*start_routine)(void *);
    void *arg;

    char *stack;

    struct __pthread_tls_datum *tls_head;

    __pthread_cleanup_datum_t *cleanup_top;
} pthread_thread_t;

static pthread_thread_t *volatile pthread_sched_threads[MAXTHREADS];
static mutex_t pthread_mutex;

#ifdef HAVE_MALLOC_H
static volatile kernel_pid_t pthread_reaper_pid = KERNEL_PID_UNDEF;
static char pthread_reaper_stack[PTHREAD_REAPER_STACKSIZE];
#endif

static void *pthread_start_routine(void *pt_)
{
    pthread_thread_t *pt = pt_;
    void *retval = pt->start_routine(pt->arg);

    pthread_exit(retval);
}

#ifdef HAVE_MALLOC_H
static void *pthread_reaper(void *arg)
{
    (void)arg;
    DEBUG("pthread_reaper(): start\n");

    while (1) {
        msg_t m;
        msg_receive(&m);
        DEBUG("pthread_reaper(): free(%p)\n", m.content.ptr);
        free(m.content.ptr);
    }

    return NULL;
}
#endif

static pthread_thread_t *_get_pthread_thread_t(pthread_t th)
{
    DEBUG("_get_pthread_thread_t()\n");
    if (!pid_is_valid(th)) {
        DEBUG("_get_pthread_thread_t(): PID not valid\n");
        return NULL;
    }
    pthread_thread_t *pt = pthread_sched_threads[th - KERNEL_PID_FIRST];

    return pt;

}

#ifdef HAVE_MALLOC_H
static int pthread_start_reaper(void)
{
    DEBUG("pthread_start_reaper()\n");
    if (pthread_reaper_pid == KERNEL_PID_UNDEF) {
        mutex_lock(&pthread_mutex);
        if (pthread_reaper_pid == KERNEL_PID_UNDEF) {
            /* volatile pid to overcome problems with double checking */
            volatile kernel_pid_t pid = thread_create(pthread_reaper_stack,
                                                      PTHREAD_REAPER_STACKSIZE,
                                                      0,
                                                      THREAD_CREATE_STACKTEST,
                                                      pthread_reaper,
                                                      NULL,
                                                      "pthread-reaper");
            if (!pid_is_valid(pid)) {
                mutex_unlock(&pthread_mutex);
                return 1;
            }
            pthread_reaper_pid = pid;
            DEBUG("pthread_start_reaper(): starting pthread_reaper\n");
        }
        mutex_unlock(&pthread_mutex);
    }
    return 0;
}
#endif

static pthread_thread_t *pthread_init_pthread_struct(const pthread_attr_t *attr, void *(*start_routine)(
                                                         void *), void *arg, char **stack_return,
                                                     int *stack_size_return )
{
    DEBUG("pthread_init_pthread_struct()\n");
    char *stack = NULL;
    int stack_size = 0;
    int autofree = 0;

    if (attr == NULL || attr->ss_sp == NULL || attr->ss_size == 0) {
        #ifndef HAVE_MALLOC_H
        DEBUG("pthread_init_pthread_struct(): no stack\n");
        return NULL;
        #else
        autofree = 1;
        /* create a stack */
        DEBUG("pthread_init_pthread_struct(): malloc memory\n");
        stack_size = attr &&
                     attr->ss_size >
                     0 ? attr->ss_size : PTHREAD_STACKSIZE;
        stack = malloc(stack_size);
        if (stack == NULL) {
            return NULL;
        }
        if (attr != NULL && attr->detached == PTS_DETACHED) {
            if (pthread_start_reaper() == 1) {
                free(stack);
                return NULL;
            }
        }
        #endif
    }else
    {
        /* has a stack in attr */
        stack = attr->ss_sp;
        stack_size = attr->ss_size;
    }

    DEBUG("pthread_init_pthread_struct(): align the stack\n");
    /* align the stack on a 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t)stack % ALIGN_OF(void *);
    if (misalignment) {
        misalignment = ALIGN_OF(void *) - misalignment;
        stack += misalignment;
        stack_size -= misalignment;
    }

    /* make room for the thread control block */
    stack_size -= sizeof(pthread_thread_t);

    /* round down the stacksize to a multiple of pthread_thread_t alignments */
    stack_size -= stack_size % ALIGN_OF(pthread_thread_t);

    if (stack_size < 0) {
        DEBUG("pthread_init_pthread_struct(): stack size to small\n");
        #ifdef HAVE_MALLOC_H
        if (autofree == 1) {
            free(stack);
        }
        #endif
        return NULL;
    }
    /* allocate our pthread control block at the top of our stackspace */
    pthread_thread_t *pt = (pthread_thread_t *)(stack + stack_size);

    /* init pt */
    DEBUG("pthread_init_pthread_struct(): init pt\n");
    if (autofree == 1) {
        pt->stack = stack;
    }
    else {
        pt->stack = NULL;
    }
    pt->arg = arg;
    pt->start_routine = start_routine;
    pt->status = attr && attr->detached ? PTS_DETACHED : PTS_JOINABLE;
    pt->thread_pid = KERNEL_PID_UNDEF;
    pt->tls_head = NULL;
    pt->cleanup_top = NULL;
    pt->returnval = NULL;
    pt->should_cancel = 0;
    mutex_init(&pt->join_mutex);
    mutex_trylock(&pt->join_mutex);

    *stack_return = stack;
    *stack_size_return = stack_size;
    return pt;
}

int pthread_create(pthread_t *newthread, const pthread_attr_t *attr, void *(*start_routine)(
                       void *), void *arg)
{
    DEBUG("pthread_create()\n");
    char *stack;
    int stack_size;
    pthread_thread_t *pt = pthread_init_pthread_struct(attr, start_routine, arg,
                                                       &stack, &stack_size);

    if (pt == NULL) {
        return -ENOMEM;
    }

    DEBUG("pthread_create(): create thread\n");
    pt->thread_pid = thread_create(stack,
                                   stack_size,
                                   THREAD_PRIORITY_MAIN,
                                   THREAD_CREATE_SLEEPING |
                                   THREAD_CREATE_STACKTEST,
                                   pthread_start_routine,
                                   pt,
                                   "pthread");
    if (!pid_is_valid(pt->thread_pid)) {
        #ifdef HAVE_MALLOC_H
        free(pt->stack);
        #endif
        return -1;
    }
    mutex_lock(&pthread_mutex);
    pthread_sched_threads[pt->thread_pid - KERNEL_PID_FIRST] = pt;
    mutex_unlock(&pthread_mutex);
    *newthread = pt->thread_pid;

    DEBUG("pthread_create(): wake up thread\n");
    thread_wakeup(pt->thread_pid);

    return 0;
}

void pthread_exit(void *retval)
{
    DEBUG("pthread_exit()\n");
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
    }
    else {
        pthread_thread_t *self = _get_pthread_thread_t(self_id);

        DEBUG("pthread_exit(): cleanup\n");
        while (self->cleanup_top) {
            DEBUG("pthread_exit(): cleanup in while\n");
            __pthread_cleanup_datum_t *ct = self->cleanup_top;


            DEBUG("pthread_exit(): cleanup in while 1\n");
            self->cleanup_top = ct->__next;
            DEBUG("pthread_exit(): cleanup in while 2\n");
            ct->__routine(ct->__arg);
        }

        DEBUG("pthread_exit(): keys\n");
        /* Prevent linking in pthread_tls.o if no TSS functions were used. */
        extern void __pthread_keys_exit(int self_id) __attribute__((weak));
        if (__pthread_keys_exit) {
            __pthread_keys_exit(self_id);
        }

        DEBUG("pthread_exit(%p), self == %p\n", retval, (void *)self);
        mutex_lock(&pthread_mutex);
        if (self->status != PTS_DETACHED) {
            self->returnval = retval;
            self->status = PTS_ZOMBIE;
            DEBUG("pthread_exit: zombify thread\n");

            irq_disable();
            mutex_unlock(&self->join_mutex);
            mutex_unlock(&pthread_mutex);
            thread_zombify();
        }
        else {
            DEBUG("pthread_exit: kill detached thread\n");
            irq_disable();
            self->thread_pid = KERNEL_PID_UNDEF;
            mutex_unlock(&pthread_mutex);
            #ifdef HAVE_MALLOC_H
            if (self->stack) {
                msg_t m;
                m.content.ptr = self->stack;
                msg_send_int(&m, pthread_reaper_pid);
            }
            #endif
        }

    }

    sched_task_exit();
}

int pthread_join(pthread_t th, void **thread_return)
{
    DEBUG("pthread_join()\n");
    pthread_thread_t *other = _get_pthread_thread_t(th);

    if (!other) {
        return -ESRCH;
    }
    if (other->status == PTS_DETACHED) {
        DEBUG("pthread_join: thread is detached\n");
        return -EINVAL;
    }

    if (other->status == PTS_JOINABLE) {
        DEBUG("pthread_join: lock mutex of thread\n");
        mutex_lock(&other->join_mutex);
    }
    if (other->status == PTS_ZOMBIE) {
        DEBUG("pthread_join: wait until thread is a zombie\n");
        while (thread_kill_zombie(other->thread_pid) != 1) {
            thread_yield();
        }

        if (thread_return) {
            *thread_return = other->returnval;
        }
        #ifdef HAVE_MALLOC_H
        if (other->stack) {
            DEBUG("pthread_join: free stack\n");
            free(other->stack);
        }
        #endif
        mutex_lock(&pthread_mutex);
        pthread_sched_threads[th - KERNEL_PID_FIRST] = NULL;
        mutex_unlock(&pthread_mutex);
        return 0;
    }

    return -2;
}

int pthread_detach(pthread_t th)
{
    DEBUG("pthread_detach()\n");
    pthread_thread_t *other = _get_pthread_thread_t(th);

    if (!other) {
        return -ESRCH;
    }

    mutex_lock(&pthread_mutex);
    if (other->status == PTS_ZOMBIE) {
        #ifdef HAVE_MALLOC_H
        if (other->stack) {
            free(other->stack);
        }
        #endif
        pthread_sched_threads[th - KERNEL_PID_FIRST] = NULL;
        mutex_unlock(&pthread_mutex);
        thread_kill_zombie(other->thread_pid);
    }
    else {
        other->status = PTS_DETACHED;
        mutex_unlock(&pthread_mutex);
        if (pthread_start_reaper() == 1) {
                return -EINVAL;
            }
    }

    return 0;
}

pthread_t pthread_self(void)
{
    DEBUG("pthread_self()\n");
    kernel_pid_t pid = thread_getpid();
    pthread_t result = (pthread_t)pid;

    return result;
}

int pthread_cancel(pthread_t th)
{
    DEBUG("pthread_cancel()\n");
    pthread_thread_t *other = pthread_sched_threads[th - 1];
    if (!other) {
        return -1;
    }

    other->should_cancel = 1;

    return 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    (void)state;
    (void)oldstate;
    DEBUG("pthread_setcancelstate(): not implemented!\n");
    return -1;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    (void)type;
    (void)oldtype;
    DEBUG("pthread_setcanceltype(): not implemented!\n");
    return -1;
}

void pthread_testcancel(void)
{
    DEBUG("pthread_testcancel()\n");
    pthread_t self = pthread_self();

    if (self == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    if (pthread_sched_threads[self - 1]->should_cancel) {
        pthread_exit(PTHREAD_CANCELED);
    }
}

void __pthread_cleanup_push(__pthread_cleanup_datum_t *datum)
{
    DEBUG("__pthread_cleanup_push()\n");
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    pthread_thread_t *self = _get_pthread_thread_t(self_id);
    datum->__next = self->cleanup_top;
    self->cleanup_top = datum;
}

void __pthread_cleanup_pop(__pthread_cleanup_datum_t *datum, int execute)
{
    DEBUG("__pthread_cleanup_pop()\n");
    pthread_t self_id = pthread_self();

    if (self_id == 0) {
        DEBUG("ERROR called pthread_self() returned 0 in \"%s\"!\n", __func__);
        return;
    }

    pthread_thread_t *self = _get_pthread_thread_t(self_id);
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
    DEBUG("__pthread_tls_datum()\n");
    pthread_thread_t *self = _get_pthread_thread_t(self_id);

    return self ? &self->tls_head : NULL;
}
