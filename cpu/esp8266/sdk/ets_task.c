/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * PLEASE NOTE: This file is only used in SDK version
 */

/*
 * Internally, the SDK uses its own priority-based multitasking system,
 * the *ETS*, to handle hardware components such as the WiFi interface, or to
 * implement event-driven functions such as software timers. ETS periodically
 * executes all ETS tasks with pending events in an infinite loop with the ROM
 * function *ets_run*.
 *
 * ETS doesn't process interrupts directly in interrupt service routines.
 * Instead, they use the *ets_post* ROM function to send an event to one of the
 * ETS tasks, which then processes the interrupts asynchronously. Context
 * switches are not possible in interrupt service routines.
 *
 * To use SDK functions and keep the system alive, ETS tasks with pending
 * events have to be handled. For that purpose
 *
 * - the *ets_task_func* RIOT thread with highest possible priority is used
 * - the ROM functions *ets_run* and *ets_post* are overwritten.
 *
 * The *ets_task_func* RIOT thread is waiting for a thread flag, which is set
 * by the *ets_post function* at the end of an ETS interrupt service routine.
 * The flag indicates that there are ETS tasks with pending events that need
 * to be executed. The *ets_task_func* RIOT thread then calls the *ets_run*
 * function, which performs all ETS tasks with pending events exactly once.
 *
 * Thus, when a hardware component used by the SDK triggers an interrupt, e.g.
 * the WiFi interface, the interrupt sevice routine posts an event to the ETS
 * task by calling the *ets_post* function. The overwritten version of this
 * function sets the thread flag of the *ets_task_func* thread. The thread
 * then calls function *ets_run* to process pending events.
 */

#ifdef MODULE_ESP_SDK

#define ENABLE_DEBUG 0
#include <stdio.h>

#include "irq_arch.h"
#include "mutex.h"
#include "thread.h"

#include "esp/common_macros.h"
#include "sdk/ets_task.h"
#include "sdk/sdk.h"

static uint8_t min_prio = 0;

/* helper function for *ets_run* */
uint8_t ets_highest_1_bit (uint32_t mask)
{
    __asm__ volatile ("nsau %0, %1;" :"=r"(mask) : "r"(mask));
    return 32 - mask;
}

/*
 * Perform the execution of all pending ETS tasks. This is necessary to
 * keep the underlying ETS system used by the SDK alive. It is called from
 * the RIOT thread *ets_task_func*.
 */
void IRAM ets_tasks_run (void)
{
    #if ENABLE_DEBUG
    uint32_t _entry = phy_get_mactime();
    uint32_t _exit;
    ets_printf("ets_tasks_run @%lu\n", _entry);
    #endif

    /* reset hardware watchdog here */
    system_soft_wdt_feed();

    while (1) {
        uint8_t  hbit;
        int state = irq_disable();
        hbit = ets_highest_1_bit (ets_task_exec_mask);
        if (min_prio < hbit) {
            ets_task_tcb_t* task  = &ets_task_tab[hbit-1];
            ETSEvent *      event = &task->queue[task->qposr++];
            if (task->qposr == task->qlength) {
                task->qposr = 0;
            }
            if (--task->qpending == 0) {
                ets_task_exec_mask &= ~task->maskbit;
            }
            ets_task_min_prio = hbit;
            irq_restore(state);
            task->task(event);
            ets_task_min_prio = min_prio;
        }
        else {
            irq_restore(state);
            break;
        }
    }

    #if ENABLE_DEBUG
    _exit = phy_get_mactime();
    ets_printf("ets_tasks_run @%lu for %lu us\n", _entry, _exit - _entry);
    #endif

    /* reset hardware watchdog here again */
    system_soft_wdt_feed();
}


#define THREAD_FLAG_ETS_THREAD (1 << 0)
static volatile thread_t* ets_thread = NULL;

/*
 * Thread *ets_task_func* is waiting for the thread flag THREAD_FLAG_ETS_THREAD
 * indicating that ETS tasks have pending events and need to be executed. When
 * the thread flag is set, it calls the *ets_run* function, which performs
 * all ETS tasks with pending events exactly once. The thread flag is set by
 * the *ets_post* function, which is called at the end of an ETS interrupt
 * service routine.
 */
void *ets_task_func(void *arg)
{
    (void) arg;

    ets_thread = sched_active_thread;
    while (1) {
        thread_flags_wait_one(THREAD_FLAG_ETS_THREAD);
        ets_tasks_run();
    }

    return NULL;
}

/* helper macro for *ets_post */
#define irom_cache_enabled() (*((uint32_t*)0x60000208) & (1 << 17))

/* ETS timer task priority */
#define TIMER_TASK_PRIORITY 31

/* reference to the *ets_post* ROM function */
typedef uint32_t (*ets_post_function_t)(uint32_t prio, ETSSignal sig, ETSParam par);
static ets_post_function_t ets_post_rom = (ets_post_function_t)0x40000e24;

/*
 * Overwritten version of ROM function *ets_post*.
 *
 * ETS doesn't process interrupts directly in interrupt service routines.
 * Instead, they use the *ets_post* ROM function to send an event to one of the
 * ETS tasks, which then processes the interrupts asynchronously. Context
 * switches are not possible in interrupt service routines.
 *
 * Please note: *ets_post* is executed in interrupt context
 */
uint32_t IRAM ets_post (uint32_t prio, ETSSignal sig, ETSParam par)
{
    /* This function is executed in interrupt context */
    uint32_t ret;

    critical_enter();

    /* test whether we are in hardware timer interrupt handling routine */
    if (prio == TIMER_TASK_PRIORITY) {
        /* first call ETS system post function */
        ret = ets_post_rom (prio, sig, par);

        /* handle only pending timer events */
        if (irom_cache_enabled()) {
            ets_timer_handler_isr();
        }
    }
    else {
        /* simply call ROM ets_post function */
        ret = ets_post_rom (prio, sig, par);
    }
    /* since only timer events are handled we have to reset watch dog timer */
    if (irom_cache_enabled()) {
        system_soft_wdt_feed();
    }

    if (ets_thread && irom_cache_enabled()) {
        thread_flags_set((thread_t*)ets_thread, THREAD_FLAG_ETS_THREAD);
    }

    critical_exit();

    return ret;
}

void ets_tasks_init(void)
{
    /* there is nothing to be done here at the moment */
}

#endif /* MODULE_ESP_SDK */
