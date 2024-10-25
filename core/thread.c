/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 * @{
 *
 * @file
 * @brief       Threading implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdalign.h>
#include <stdio.h>
#ifdef PICOLIBC_TLS
#include <picotls.h>
#endif

#include "assert.h"
#include "thread.h"
#include "irq.h"

#include "bitarithm.h"
#include "sched.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(HAVE_VALGRIND_H)
#  include <valgrind.h>
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#  include <valgrind/valgrind.h>
#else
#  define   VALGRIND_DISABLE_ERROR_REPORTING    (void)0
#  define   VALGRIND_ENABLE_ERROR_REPORTING     (void)0
#endif

thread_status_t thread_getstatus(kernel_pid_t pid)
{
    thread_t *thread = thread_get(pid);

    return thread ? thread->status : STATUS_NOT_FOUND;
}

const char *thread_getname(kernel_pid_t pid)
{
#ifdef CONFIG_THREAD_NAMES
    thread_t *thread = thread_get(pid);
    return thread ? thread->name : NULL;
#else
    (void)pid;
    return NULL;
#endif
}

void thread_zombify(void)
{
    if (irq_is_in()) {
        return;
    }

    irq_disable();
    sched_set_status(thread_get_active(), STATUS_ZOMBIE);
    irq_enable();
    thread_yield_higher();

    /* this line should never be reached */
    UNREACHABLE();
}

int thread_kill_zombie(kernel_pid_t pid)
{
    DEBUG("thread_kill: Trying to kill PID %" PRIkernel_pid "...\n", pid);
    unsigned state = irq_disable();

    int result = (int)STATUS_NOT_FOUND;

    thread_t *thread = thread_get(pid);

    if (!thread) {
        DEBUG("thread_kill: Thread does not exist!\n");
    }
    else if (thread->status == STATUS_ZOMBIE) {
        DEBUG("thread_kill: Thread is a zombie.\n");

        sched_threads[pid] = NULL;
        sched_num_threads--;
        sched_set_status(thread, STATUS_STOPPED);

        result =  1;
    }
    else {
        DEBUG("thread_kill: Thread is not a zombie!\n");
    }
    irq_restore(state);
    return result;
}

void thread_sleep(void)
{
    if (irq_is_in()) {
        return;
    }

    assert(irq_is_enabled());
    irq_disable();

    sched_set_status(thread_get_active(), STATUS_SLEEPING);
    irq_enable();
    thread_yield_higher();
}

int thread_wakeup(kernel_pid_t pid)
{
    DEBUG("thread_wakeup: Trying to wakeup PID %" PRIkernel_pid "...\n", pid);

    unsigned old_state = irq_disable();

    thread_t *thread = thread_get(pid);

    if (!thread) {
        DEBUG("thread_wakeup: Thread does not exist!\n");
    }
    else if (thread->status == STATUS_SLEEPING) {
        DEBUG("thread_wakeup: Thread is sleeping.\n");

        sched_set_status(thread, STATUS_RUNNING);

        irq_restore(old_state);
        sched_switch(thread->priority);

        return 1;
    }
    else {
        DEBUG("thread_wakeup: Thread is not sleeping!\n");
    }

    irq_restore(old_state);
    return (int)STATUS_NOT_FOUND;
}

void thread_yield(void)
{
    assume(irq_is_in() || irq_is_enabled());
    irq_disable();
    thread_t *me = thread_get_active();

    if (me->status >= STATUS_ON_RUNQUEUE) {
        sched_runq_advance(me->priority);
    }
    irq_enable();

    thread_yield_higher();
}

void thread_add_to_list(list_node_t *list, thread_t *thread)
{
    assert(thread->status < STATUS_ON_RUNQUEUE);

    uint16_t my_prio = thread->priority;
    list_node_t *new_node = (list_node_t *)&thread->rq_entry;

    while (list->next) {
        thread_t *list_entry = container_of((clist_node_t *)list->next,
                                            thread_t, rq_entry);
        if (list_entry->priority > my_prio) {
            break;
        }
        list = list->next;
    }

    new_node->next = list->next;
    list->next = new_node;
}

uintptr_t measure_stack_free_internal(const char *stack, size_t size)
{
    /* Alignment of stack has been fixed (if needed) by thread_create(), so
     * we can silence -Wcast-align here */
    uintptr_t *stackp = (uintptr_t *)(uintptr_t)stack;
    uintptr_t end = (uintptr_t)stack + size;

    /* HACK: This will affect native/native64 only.
     *
     * The dark magic used here is frowned upon by valgrind. E.g. valgrind may
     * deduce that a specific value was at some point allocated on the stack,
     * but has gone out of scope. When that value is now read again to
     * estimate stack usage, it does look a lot like someone passed a pointer
     * to a stack allocated value, and that pointer is referenced after that
     * value has gone out of scope. */
    VALGRIND_DISABLE_ERROR_REPORTING;

    /* assume that the stack grows "downwards" */
    while (((uintptr_t)stackp < end) && (*stackp == (uintptr_t)stackp)) {
        stackp++;
    }

    VALGRIND_ENABLE_ERROR_REPORTING;

    uintptr_t space_free = (uintptr_t)stackp - (uintptr_t)stack;

    return space_free;
}

kernel_pid_t thread_create(char *stack, int stacksize, uint8_t priority,
                           int flags, thread_task_func_t function, void *arg,
                           const char *name)
{
    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

#ifdef DEVELHELP
    int total_stacksize = stacksize;
#endif
#ifndef CONFIG_THREAD_NAMES
    (void)name;
#endif

    /* align the stack on a 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t)stack % alignof(void *);
    if (misalignment) {
        misalignment = alignof(void *) - misalignment;
        stack += misalignment;
        stacksize -= misalignment;
    }

    /* make room for the thread control block */
    stacksize -= sizeof(thread_t);

    /* round down the stacksize to a multiple of thread_t alignments (usually 16/32bit) */
    stacksize -= stacksize % alignof(thread_t);

    if (stacksize < 0) {
        DEBUG("thread_create: stacksize is too small!\n");
    }
    /* allocate our thread control block at the top of our stackspace. Cast to
     * (uintptr_t) intermediately to silence -Wcast-align. (We manually made
     * sure alignment is correct above.) */
    thread_t *thread = (thread_t *)(uintptr_t)(stack + stacksize);

#ifdef PICOLIBC_TLS
#if __PICOLIBC_MAJOR__ > 1 || __PICOLIBC_MINOR__ >= 8
#define TLS_ALIGN       (alignof(thread_t) > _tls_align() ? alignof(thread_t) : _tls_align())
#else
#define TLS_ALIGN       alignof(thread_t)
#endif
    char *tls = stack + stacksize - _tls_size();
    /*
     * Make sure the TLS area is aligned as required and that the
     * resulting stack will also be aligned as required
     */
    thread->tls = (void *) ((uintptr_t) tls & ~ (TLS_ALIGN - 1));
    stacksize = (char *) thread->tls - stack;

    _init_tls(thread->tls);
#endif

#if defined(DEVELHELP) || defined(SCHED_TEST_STACK) \
    || defined(MODULE_TEST_UTILS_PRINT_STACK_USAGE)
    if (flags & THREAD_CREATE_NO_STACKTEST) {
        /* create stack guard. Alignment has been handled above, so silence
         * -Wcast-align */
        *(uintptr_t *)(uintptr_t)stack = (uintptr_t)stack;
    }
    else {
        /* assign each int of the stack the value of it's address. Alignment
         * has been handled above, so silence -Wcast-align */
        uintptr_t *stackmax = (uintptr_t *)(uintptr_t)(stack + stacksize);
        uintptr_t *stackp = (uintptr_t *)(uintptr_t)stack;

        while (stackp < stackmax) {
            *stackp = (uintptr_t)stackp;
            stackp++;
        }
    }
#endif

    unsigned state = irq_disable();

    kernel_pid_t pid = KERNEL_PID_UNDEF;
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i) {
        if (sched_threads[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid == KERNEL_PID_UNDEF) {
        DEBUG("thread_create(): too many threads!\n");

        irq_restore(state);

        return -EOVERFLOW;
    }

    sched_threads[pid] = thread;

    thread->pid = pid;
    thread->sp = thread_stack_init(function, arg, stack, stacksize);

#if defined(DEVELHELP) || IS_ACTIVE(SCHED_TEST_STACK) || \
    defined(MODULE_MPU_STACK_GUARD) || defined(MODULE_CORTEXM_STACK_LIMIT)
    thread->stack_start = stack;
#endif

#ifdef DEVELHELP
    thread->stack_size = total_stacksize;
#endif
#ifdef CONFIG_THREAD_NAMES
    thread->name = name;
#endif

    thread->priority = priority;
    thread->status = STATUS_STOPPED;

    thread->rq_entry.next = NULL;

#ifdef MODULE_CORE_MSG
    thread->wait_data = NULL;
    thread->msg_waiters.next = NULL;
    cib_init(&(thread->msg_queue), 0);
    thread->msg_array = NULL;
#endif

    sched_num_threads++;

    DEBUG("Created thread %s. PID: %" PRIkernel_pid ". Priority: %u.\n", name,
          thread->pid, priority);

    if (flags & THREAD_CREATE_SLEEPING) {
        sched_set_status(thread, STATUS_SLEEPING);
    }
    else {
        sched_set_status(thread, STATUS_PENDING);

        if (!(flags & THREAD_CREATE_WOUT_YIELD)) {
            irq_restore(state);
            sched_switch(priority);
            return pid;
        }
    }

    irq_restore(state);

    return pid;
}

static const char *state_names[STATUS_NUMOF] = {
    [STATUS_STOPPED] = "stopped",
    [STATUS_ZOMBIE] = "zombie",
    [STATUS_SLEEPING] = "sleeping",
    [STATUS_MUTEX_BLOCKED] = "bl mutex",
    [STATUS_RECEIVE_BLOCKED] = "bl rx",
    [STATUS_SEND_BLOCKED] = "bl send",
    [STATUS_REPLY_BLOCKED] = "bl reply",
    [STATUS_FLAG_BLOCKED_ANY] = "bl anyfl",
    [STATUS_FLAG_BLOCKED_ALL] = "bl allfl",
    [STATUS_MBOX_BLOCKED] = "bl mbox",
    [STATUS_COND_BLOCKED] = "bl cond",
    [STATUS_RUNNING] = "running",
    [STATUS_PENDING] = "pending",
};

#define STATE_NAME_UNKNOWN "unknown"

const char *thread_state_to_string(thread_status_t state)
{
    const char *name = state_names[state] ? state_names[state] : NULL;

    assert(name != NULL); /* if compiling with assertions, this is an error that
                             indicates that the table above is incomplete */

    return (name != NULL) ? name : STATE_NAME_UNKNOWN;
}
