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

#include "sched.h"
#include "clist.h"
#include "bitarithm.h"
#include "irq.h"
#include "thread.h"
#include "irq.h"
#include "log.h"

#ifdef MODULE_MPU_STACK_GUARD
#include "mpu.h"
#endif

#ifdef MODULE_SCHEDSTATISTICS
#include "xtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

volatile int sched_num_threads = 0;

volatile unsigned int sched_context_switch_request;

volatile thread_t *sched_threads[KERNEL_PID_LAST + 1];
volatile thread_t *sched_active_thread;

volatile kernel_pid_t sched_active_pid = KERNEL_PID_UNDEF;

clist_node_t sched_runqueues[SCHED_PRIO_LEVELS];
static uint32_t runqueue_bitcache = 0;

/* Needed by OpenOCD to read sched_threads */
#if defined(__APPLE__) && defined(__MACH__)
 #define FORCE_USED_SECTION __attribute__((used)) __attribute__((section ("__OPENOCD,__openocd")))
#else
 #define FORCE_USED_SECTION __attribute__((used)) __attribute__((section (".openocd")))
#endif

FORCE_USED_SECTION
uint8_t max_threads = sizeof(sched_threads) / sizeof(thread_t*);

#ifdef DEVELHELP
/* OpenOCD can't determine struct offsets and additionally this member is only
 * available if compiled with DEVELHELP */
FORCE_USED_SECTION
uint8_t _tcb_name_offset = offsetof(thread_t, name);
#endif

#ifdef MODULE_SCHEDSTATISTICS
static void (*sched_cb) (uint32_t timestamp, uint32_t value) = NULL;
schedstat sched_pidlist[KERNEL_PID_LAST + 1];
#endif

int __attribute__((used)) sched_run(void)
{
    sched_context_switch_request = 0;

    thread_t *active_thread = (thread_t *)sched_active_thread;

    /* The bitmask in runqueue_bitcache is never empty,
     * since the threading should not be started before at least the idle thread was started.
     */
    int nextrq = bitarithm_lsb(runqueue_bitcache);
    thread_t *next_thread = container_of(sched_runqueues[nextrq].next->next, thread_t, rq_entry);

    DEBUG("sched_run: active thread: %" PRIkernel_pid ", next thread: %" PRIkernel_pid "\n",
          (active_thread == NULL) ? KERNEL_PID_UNDEF : active_thread->pid,
          next_thread->pid);

    if (active_thread == next_thread) {
        DEBUG("sched_run: done, sched_active_thread was not changed.\n");
        return 0;
    }

#ifdef MODULE_SCHEDSTATISTICS
    uint64_t now = _xtimer_now64();
#endif

    if (active_thread) {
        if (active_thread->status == STATUS_RUNNING) {
            active_thread->status = STATUS_PENDING;
        }

#ifdef SCHED_TEST_STACK
        if (*((uintptr_t *) active_thread->stack_start) != (uintptr_t) active_thread->stack_start) {
            LOG_WARNING("scheduler(): stack overflow detected, pid=%" PRIkernel_pid "\n", active_thread->pid);
        }
#endif

#ifdef MODULE_SCHEDSTATISTICS
        schedstat *active_stat = &sched_pidlist[active_thread->pid];
        if (active_stat->laststart) {
            active_stat->runtime_ticks += now - active_stat->laststart;
        }
#endif
    }

#ifdef MODULE_SCHEDSTATISTICS
    schedstat *next_stat = &sched_pidlist[next_thread->pid];
    next_stat->laststart = now;
    next_stat->schedules++;
    if (sched_cb) {
        sched_cb(now, next_thread->pid);
    }
#endif

    next_thread->status = STATUS_RUNNING;
    sched_active_pid = next_thread->pid;
    sched_active_thread = (volatile thread_t *) next_thread;

#ifdef MODULE_MPU_STACK_GUARD
    mpu_configure(
        1,                                                /* MPU region 1 */
        (uintptr_t)sched_active_thread->stack_start + 31, /* Base Address (rounded up) */
        MPU_ATTR(1, AP_RO_RO, 0, 1, 0, 1, MPU_SIZE_32B)   /* Attributes and Size */
    );

    mpu_enable();
#endif

    DEBUG("sched_run: done, changed sched_active_thread.\n");

    return 1;
}

#ifdef MODULE_SCHEDSTATISTICS
void sched_register_cb(void (*callback)(uint32_t, uint32_t))
{
    sched_cb = callback;
}
#endif

void sched_set_status(thread_t *process, unsigned int status)
{
    if (status >= STATUS_ON_RUNQUEUE) {
        if (!(process->status >= STATUS_ON_RUNQUEUE)) {
            DEBUG("sched_set_status: adding thread %" PRIkernel_pid " to runqueue %" PRIu16 ".\n",
                  process->pid, process->priority);
            clist_rpush(&sched_runqueues[process->priority], &(process->rq_entry));
            runqueue_bitcache |= 1 << process->priority;
        }
    }
    else {
        if (process->status >= STATUS_ON_RUNQUEUE) {
            DEBUG("sched_set_status: removing thread %" PRIkernel_pid " to runqueue %" PRIu16 ".\n",
                  process->pid, process->priority);
            clist_lpop(&sched_runqueues[process->priority]);

            if (!sched_runqueues[process->priority].next) {
                runqueue_bitcache &= ~(1 << process->priority);
            }
        }
    }

    process->status = status;
}

void sched_switch(uint16_t other_prio)
{
    thread_t *active_thread = (thread_t *) sched_active_thread;
    uint16_t current_prio = active_thread->priority;
    int on_runqueue = (active_thread->status >= STATUS_ON_RUNQUEUE);

    DEBUG("sched_switch: active pid=%" PRIkernel_pid" prio=%" PRIu16 " on_runqueue=%i "
          ", other_prio=%" PRIu16 "\n",
          active_thread->pid, current_prio, on_runqueue, other_prio);

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
    DEBUG("sched_task_exit: ending thread %" PRIkernel_pid "...\n", sched_active_thread->pid);

    (void) irq_disable();
    sched_threads[sched_active_pid] = NULL;
    sched_num_threads--;

    sched_set_status((thread_t *)sched_active_thread, STATUS_STOPPED);

    sched_active_thread = NULL;
    cpu_switch_context_exit();
}
