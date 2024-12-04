/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
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
 * @brief       Platform-independent kernel initialization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "auto_init.h"
#include "irq.h"
#include "kernel_init.h"
#include "log.h"
#include "periph/pm.h"
#include "thread.h"
#include "stdio_base.h"

#if IS_USED(MODULE_VFS)
#include "vfs.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_BOOT_MSG_STRING
#define CONFIG_BOOT_MSG_STRING "main(): This is RIOT! (Version: " \
    RIOT_VERSION ")"
#endif

static char main_stack[THREAD_STACKSIZE_MAIN];
static char idle_stack[THREAD_STACKSIZE_IDLE];

static void *main_trampoline(void *arg)
{
    (void)arg;

    if (IS_USED(MODULE_AUTO_INIT)) {
        auto_init();
    }

    if (!IS_ACTIVE(CONFIG_SKIP_BOOT_MSG) && !IS_USED(MODULE_STDIO_NULL)) {
        LOG_INFO(CONFIG_BOOT_MSG_STRING "\n");
    }

#ifdef CPU_NATIVE
    extern int _native_argc_main;
    extern char **_native_argv_main;
    extern int main(int argc, char **argv);
    int res = main(_native_argc_main, _native_argv_main);
#else
    extern int main(void);
    int res = main();
#endif

    if (IS_USED(MODULE_TEST_UTILS_MAIN_EXIT_CB)) {
        void test_utils_main_exit_cb(int res);
        test_utils_main_exit_cb(res);
    }

#ifdef MODULE_TEST_UTILS_PRINT_STACK_USAGE
    void print_stack_usage_metric(const char *name, void *stack, unsigned max_size);
    if (IS_USED(MODULE_CORE_IDLE_THREAD)) {
        print_stack_usage_metric("idle", idle_stack, THREAD_STACKSIZE_IDLE);
    }
#endif

#ifdef CPU_NATIVE
    extern unsigned _native_retval;
    if (!_native_retval) {
        _native_retval = res;
    }
#endif

    if (IS_ACTIVE(CONFIG_CORE_EXIT_WITH_MAIN) && IS_USED(MODULE_PERIPH_PM)) {
        pm_off();
    }

    return NULL;
}

static void *idle_thread(void *arg)
{
    (void)arg;

    while (1) {
        if (IS_USED(MODULE_PERIPH_PM)) {
            pm_set_lowest();
        }
    }

    return NULL;
}

void kernel_init(void)
{
    if (!IS_USED(MODULE_CORE_THREAD)) {
        /* RIOT without threads */
        main_trampoline(NULL);
        while (1) {}
        return;
    }

    irq_disable();

    if (IS_USED(MODULE_CORE_IDLE_THREAD)) {
        thread_create(idle_stack, sizeof(idle_stack),
                      THREAD_PRIORITY_IDLE,
                      THREAD_CREATE_WOUT_YIELD,
                      idle_thread, NULL, "idle");
    }

    thread_create(main_stack, sizeof(main_stack),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_WOUT_YIELD,
                  main_trampoline, NULL, "main");

    cpu_switch_context_exit();
}

void early_init(void)
{
    /* initialize leds */
    if (IS_USED(MODULE_PERIPH_INIT_LEDS)) {
        extern void led_init(void);
        led_init();
    }

    stdio_init();

#if MODULE_VFS
    vfs_bind_stdio();
#endif
}
