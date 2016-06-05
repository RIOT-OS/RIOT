/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Platform-independent kernel initilization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "kernel_init.h"
#include "sched.h"
#include "thread.h"
#include "lpm.h"
#include "irq.h"
#include "log.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "sched.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_AUTO_INIT
#include <auto_init.h>
#endif

#ifdef MODULE_PREINIT
#include "init.h"
extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
#endif

volatile int lpm_prevent_sleep = 0;

extern int main(void);
static void *main_trampoline(void *arg)
{
    (void) arg;

#ifdef MODULE_AUTO_INIT
    auto_init();
#endif

#ifdef MODULE_SCHEDSTATISTICS
    schedstat *stat = &sched_pidlist[thread_getpid()];
    stat->laststart = 0;
#endif

    LOG_INFO("main(): This is RIOT! (Version: " RIOT_VERSION ")\n");

    main();
    return NULL;
}

static void *idle_thread(void *arg)
{
    (void) arg;

    while (1) {
        if (lpm_prevent_sleep) {
            lpm_set(LPM_IDLE);
        }
        else {
            lpm_set(LPM_IDLE);
            /* lpm_set(LPM_SLEEP); */
            /* lpm_set(LPM_POWERDOWN); */
        }
    }

    return NULL;
}

const char *main_name = "main";
const char *idle_name = "idle";

static char main_stack[THREAD_STACKSIZE_MAIN];
static char idle_stack[THREAD_STACKSIZE_IDLE];

void kernel_init(void)
{
    (void) irq_disable();

#ifdef MODULE_PREINIT
    size_t array_len = __preinit_array_end - __preinit_array_start;
    for (size_t i = 0; i < array_len; i++) {
        __preinit_array_start[i] ();
    }

    array_len = __init_array_end - __init_array_start;
    for (size_t i = 0; i < array_len; i++) {
        __init_array_start[i] ();
    }
#endif

    thread_create(idle_stack, sizeof(idle_stack),
            THREAD_PRIORITY_IDLE,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            idle_thread, NULL, idle_name);

    thread_create(main_stack, sizeof(main_stack),
            THREAD_PRIORITY_MAIN,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            main_trampoline, NULL, main_name);

    cpu_switch_context_exit();
}
