/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sched
 * @{
 *
 * @file
 * @brief       Scheduler implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <inttypes.h>

#include "sched.h"
#include "clist.h"
#include "bitarithm.h"
#include "irq.h"
#include "thread.h"
#include "log.h"

#ifdef MODULE_MPU_STACK_GUARD
#include "mpu.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef PICOLIBC_TLS
#include <picotls.h>
#endif

/* Needed by OpenOCD to read sched_threads */
#if defined(__APPLE__) && defined(__MACH__)
 #define FORCE_USED_SECTION __attribute__((used)) __attribute__((section( \
                                                                     "__OPENOCD,__openocd")))
#else
 #define FORCE_USED_SECTION __attribute__((used)) __attribute__((section( \
                                                                     ".openocd")))
#endif

/**
 * @brief   Symbols also used by OpenOCD, keep in sync with src/rtos/riot.c
 * @{
 */
volatile kernel_pid_t sched_active_pid = KERNEL_PID_UNDEF;
volatile thread_t *sched_threads[KERNEL_PID_LAST + 1];
volatile int sched_num_threads = 0;

FORCE_USED_SECTION
const uint8_t max_threads = ARRAY_SIZE(sched_threads);

#ifdef DEVELHELP
/* OpenOCD can't determine struct offsets and additionally this member is only
 * available if compiled with DEVELHELP */
FORCE_USED_SECTION
const uint8_t _tcb_name_offset = offsetof(thread_t, name);
#endif
/** @} */

volatile thread_t *sched_active_thread;
volatile unsigned int sched_context_switch_request;

clist_node_t sched_runqueues[SCHED_PRIO_LEVELS];
static uint32_t runqueue_bitcache = 0;

#ifdef MODULE_SCHED_CB
static void (*sched_cb)(kernel_pid_t active_thread,
                        kernel_pid_t next_thread) = NULL;
#endif

/* Depending on whether the CLZ instruction is available, the order of the
 * runqueue_bitcache is reversed. When the instruction is available, it is
 * faster to determine the MSBit set. When it is not available it is faster to
 * determine the LSBit set. These functions abstract the runqueue modifications
 * and readout away, switching between the two orders depending on the CLZ
 * instruction availability
 */
static inline void _set_runqueue_bit(thread_t *process)
{
#if defined(BITARITHM_HAS_CLZ)
    runqueue_bitcache |= BIT31 >> process->priority;
#else
    runqueue_bitcache |= 1 << process->priority;
#endif
}

static inline void _clear_runqueue_bit(thread_t *process)
{
#if defined(BITARITHM_HAS_CLZ)
    runqueue_bitcache &= ~(BIT31 >> process->priority);
#else
    runqueue_bitcache &= ~(1 << process->priority);
#endif
}

static inline unsigned _get_prio_queue_from_runqueue(void)
{
#if defined(BITARITHM_HAS_CLZ)
    return 31 - bitarithm_msb(runqueue_bitcache);
#else
    return bitarithm_lsb(runqueue_bitcache);
#endif
}

static void _unschedule(thread_t *active_thread)
{
    if (active_thread->status == STATUS_RUNNING) {
        active_thread->status = STATUS_PENDING;
    }

#ifdef SCHED_TEST_STACK
    if (*((uintptr_t *)active_thread->stack_start) !=
        (uintptr_t)active_thread->stack_start) {
        LOG_WARNING(
            "scheduler(): stack overflow detected, pid=%" PRIkernel_pid "\n",
            active_thread->pid);
    }
#endif
#ifdef MODULE_SCHED_CB
    if (sched_cb) {
        sched_cb(active_thread->pid, KERNEL_PID_UNDEF);
    }
#endif
}

thread_t *__attribute__((used)) sched_run(void)
{
    thread_t *active_thread = thread_get_active();
    thread_t *previous_thread = active_thread;

    if (!IS_USED(MODULE_CORE_IDLE_THREAD) && !runqueue_bitcache) {
        if (active_thread) {
            _unschedule(active_thread);
            active_thread = NULL;
        }

        do {
            sched_arch_idle();
        } while (!runqueue_bitcache);
    }

    sched_context_switch_request = 0;

    unsigned nextrq = _get_prio_queue_from_runqueue();
    thread_t *next_thread = container_of(sched_runqueues[nextrq].next->next,
                                         thread_t, rq_entry);

#if (IS_USED(MODULE_SCHED_RUNQ_CALLBACK))
    sched_runq_callback(nextrq);
#endif

    DEBUG(
        "sched_run: active thread: %" PRIkernel_pid ", next thread: %" PRIkernel_pid "\n",
        (kernel_pid_t)((active_thread == NULL)
                       ? KERNEL_PID_UNDEF
                       : active_thread->pid),
        next_thread->pid);

    next_thread->status = STATUS_RUNNING;

    if (previous_thread == next_thread) {
#ifdef MODULE_SCHED_CB
        /* Call the sched callback again only if the active thread is NULL. When
         * active_thread is NULL, there was a sleep in between descheduling the
         * previous thread and scheduling the new thread. Call the callback here
         * again ensures that the time sleeping doesn't count as running the
         * previous thread
         */
        if (sched_cb && !active_thread) {
            sched_cb(KERNEL_PID_UNDEF, next_thread->pid);
        }
#endif
        DEBUG("sched_run: done, sched_active_thread was not changed.\n");
    }
    else {
        if (active_thread) {
            _unschedule(active_thread);
        }

        sched_active_pid = next_thread->pid;
        sched_active_thread = next_thread;

#ifdef MODULE_SCHED_CB
        if (sched_cb) {
            sched_cb(KERNEL_PID_UNDEF, next_thread->pid);
        }
#endif

#ifdef PICOLIBC_TLS
        _set_tls(next_thread->tls);
#endif

#ifdef MODULE_MPU_STACK_GUARD
        mpu_configure(
            2,                                              /* MPU region 2 */
            (uintptr_t)next_thread->stack_start + 31,       /* Base Address (rounded up) */
            MPU_ATTR(1, AP_RO_RO, 0, 1, 0, 1, MPU_SIZE_32B) /* Attributes and Size */
            );
#endif
        DEBUG("sched_run: done, changed sched_active_thread.\n");
    }

    return next_thread;
}

void sched_set_status(thread_t *process, thread_status_t status)
{
    if (status >= STATUS_ON_RUNQUEUE) {
        if (!(process->status >= STATUS_ON_RUNQUEUE)) {
            DEBUG(
                "sched_set_status: adding thread %" PRIkernel_pid " to runqueue %" PRIu8 ".\n",
                process->pid, process->priority);
            clist_rpush(&sched_runqueues[process->priority],
                        &(process->rq_entry));
            _set_runqueue_bit(process);

            /* some thread entered a runqueue
             * if it is the active runqueue
             * inform the runqueue_change callback */
#if (IS_USED(MODULE_SCHED_RUNQ_CALLBACK))
            thread_t *active_thread = thread_get_active();
            if (active_thread && active_thread->priority == process->priority) {
                sched_runq_callback(process->priority);
            }
#endif
        }
    }
    else {
        if (process->status >= STATUS_ON_RUNQUEUE) {
            DEBUG(
                "sched_set_status: removing thread %" PRIkernel_pid " from runqueue %" PRIu8 ".\n",
                process->pid, process->priority);
            clist_lpop(&sched_runqueues[process->priority]);

            if (!sched_runqueues[process->priority].next) {
                _clear_runqueue_bit(process);
#if (IS_USED(MODULE_SCHED_RUNQ_CALLBACK))
                sched_runq_callback(process->priority);
#endif
            }
        }
    }

    process->status = status;
}

void sched_switch(uint16_t other_prio)
{
    thread_t *active_thread = thread_get_active();
    uint16_t current_prio = active_thread->priority;
    int on_runqueue = (active_thread->status >= STATUS_ON_RUNQUEUE);

    DEBUG("sched_switch: active pid=%" PRIkernel_pid " prio=%" PRIu16 " on_runqueue=%i "
          ", other_prio=%" PRIu16 "\n",
          active_thread->pid, current_prio, on_runqueue,
          other_prio);

    if (!on_runqueue || (current_prio > other_prio)) {
        if (irq_is_in()) {
            DEBUG("sched_switch: setting sched_context_switch_request.\n");
            sched_context_switch_request = 1;
        }
        else {
            DEBUG("sched_switch: yielding immediately.\n");
            thread_yield_higher();
        }
    }
    else {
        DEBUG("sched_switch: continuing without yield.\n");
    }
}

NORETURN void sched_task_exit(void)
{
    DEBUG("sched_task_exit: ending thread %" PRIkernel_pid "...\n",
          thread_getpid());

    (void)irq_disable();
    sched_threads[thread_getpid()] = NULL;
    sched_num_threads--;

    sched_set_status(thread_get_active(), STATUS_STOPPED);

    sched_active_thread = NULL;
    cpu_switch_context_exit();
}

#ifdef MODULE_SCHED_CB
void sched_register_cb(void (*callback)(kernel_pid_t, kernel_pid_t))
{
    sched_cb = callback;
}
#endif
