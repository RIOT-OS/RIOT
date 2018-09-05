/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * PLEASE NOTE: This file is only used in SDK version
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

#define TIMER_TASK_PRIORITY 31

static uint8_t min_prio = 0;

uint8_t ets_highest_1_bit (uint32_t mask)
{
    __asm__ volatile ("nsau %0, %1;" :"=r"(mask) : "r"(mask));
    return 32 - mask;
}

/**
 * @brief   Perform execution of all pending ETS system tasks.
 *
 * This is necessary to keep the underlying ETS system used by the
 * SDK alive.
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

/**
 * To realize event-driven SDK functions such as WiFi functions and software
 * timers, and to keep the system alive, the SDK internally uses its own
 * tasks (SDK tasks) and its own scheduling mechanism. For this purpose, the
 * SDK regularly executes SDK tasks with pending events in an endless loop
 * using the ROM function *ets_run*.
 *
 * Interrupt service routines do not process interrupts directly but use
 * the *ets_post* ROM function to send an event to one of these SDK tasks,
 * which then processes the interrupts asynchronously. A context switch is
 * not possible in the interrupt service routines.
 *
 * In the RIOT port, the task management of the SDK is replaced by the task
 * management of the RIOT. To handle SDK tasks with pending events so that
 * the SDK functions work and the system keeps alive, the ROM functions
 * *ets_run* and *ets_post* are overwritten. The *ets_run* function performs
 * all SDK tasks with pending events exactly once. It is executed at the end
 * of the *ets_post* function and thus usually at the end of an SDK interrupt
 * service routine or before the system goes into the lowest power mode.
 *
 * PLEASE REMEBER: we are doing that in interrupt context
 *
 * -> it must not take to much time (how can we ensure that)?
 *
 * -> we have to indicate that we are in interrupt context see *irq_is_in*
 *    and *irq_interrupt_nesting* (as realized by the level 1 exception handler
 *    in non SDK task handling environment, option MODULE_ESP_SDK_INT_HANDLING=0,
 *    the default)
 *
 * -> we must not execute a context switch or we have to execute the context
 *    switch from interrupt as following (as realized by the level 1
 *    interrupt exception handler in non SDK task handling environment, option
 *    MODULE_ESP_SDK_INT_HANDLING=0, the default)
 *      _frxt_int_enter();
 *      _frxt_switch_context();
 *      _frxt_int_exit();
 */

typedef uint32_t (*ets_post_function_t)(uint32_t prio, ETSSignal sig, ETSParam par);

static ets_post_function_t ets_post_rom = (ets_post_function_t)0x40000e24;

#ifdef MODULE_ESP_SDK
#define irom_cache_enabled() (*((uint32_t*)0x60000208) & (1 << 17))
#else
#define irom_cache_enabled() (1)
#endif

uint32_t IRAM ets_post (uint32_t prio, ETSSignal sig, ETSParam par)
{
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

    critical_exit();

    return ret;
}

void ets_tasks_init(void)
{
    /* there is nothing to do at the moment */
}

#endif /* MODULE_ESP_SDK */
