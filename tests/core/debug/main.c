/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test debug.h with different configurations
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <stdint.h>
#include "mutex.h"
#include "ztimer.h"

#include "tests.h"

/* this would typically be set application-wide via Kconfig or make */
#define CONFIG_DEBUG_SHOW_THREAD 1

#define ENABLE_DEBUG 1
#define DEBUG_UNIT "main"
#include "debug.h"

#define MY_DEBUG(...) DEBUG_("custom", __VA_ARGS__)
#define MY_DEBUG_PUTS(str) DEBUG_PUTS_("custom", str)

static mutex_t _barrier = MUTEX_INIT_LOCKED;

static void _timer_isr(void *arg)
{
    uint8_t *value = arg;
    DEBUG_PUTS("debug puts");
    DEBUG("debug printf number '%d'", *value);
    DEBUG_CONT(" ... continued\n");
    mutex_unlock(&_barrier);
}

static uint8_t _value = 42;
static ztimer_t _timer = { .callback = _timer_isr, .arg = &_value };

int main(void)
{
    ztimer_set(ZTIMER_SEC, &_timer, 1);

    disabled();
    noprefix();

    prefix();
    prefix_func();
    prefix_thread();
    prefix_thread_func();

    mutex_lock(&_barrier);

    MY_DEBUG_PUTS("debug puts");
    MY_DEBUG("debug printf number '%d'", _value);
    DEBUG_CONT(" ... continued\n");
}
