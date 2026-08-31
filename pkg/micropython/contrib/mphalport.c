/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython HAL implementation for RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/runtime.h"

#include "stdio_base.h"
#include "thread.h"
#include "thread_flags.h"
#include "xtimer.h"

/**
 * @brief   Thread flags to wake-up the MicroPython thread
 */
#define MP_RIOT_FLAG_SCHED      (1u << 0)               /**< a callback was scheduled */
#define MP_RIOT_FLAG_STDIN      (1u << 1)               /**< a stdin byte is available */
#define MP_RIOT_FLAG_TIMEOUT    (THREAD_FLAG_TIMEOUT)   /**< a timer timeout occurred */

static thread_t *_mp_thread;

static void _stdio_notify(void *arg)
{
    if (_mp_thread != NULL) {
        thread_flags_set(_mp_thread, MP_RIOT_FLAG_STDIN);
    }
}

void mp_riot_sched_hook(void)
{
    if (_mp_thread != NULL) {
        thread_flags_set(_mp_thread, MP_RIOT_FLAG_SCHED);
    }
}

int mp_hal_stdin_rx_chr(void)
{
    int flags = 0;
    char c;

    /* this thread will block for a character or a scheduled callback, so the
     * interrupt handler needs to know which thread to wake up */
    if (_mp_thread == NULL) {
        _mp_thread = thread_get_active();
    }

    stdio_set_notify(_stdio_notify, NULL);

    while (1) {
        /* handle scheduled callbacks and pending exceptions first */
        if (flags & MP_RIOT_FLAG_SCHED) {
            mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);
        }

        /* read from the stdin buffer */
        if (stdio_available() > 0) {

            ssize_t res = stdio_read(&c, 1);

            if (res >= 0) {
                break;
            }
        }

        /* block until the next thread flag */
        flags = thread_flags_wait_any(MP_RIOT_FLAG_STDIN | MP_RIOT_FLAG_SCHED);
    }

    stdio_set_notify(NULL, NULL);

    return c;
}

void mp_hal_delay_ms(mp_uint_t ms)
{
    int flags = 0;

    /* this thread will block until an interrupt occurs, so the interrupt
     * handler needs to know which thread to wake up */
    if (_mp_thread == NULL) {
        _mp_thread = thread_get_active();
    }

    uint32_t remaining = (uint32_t)ms * 1000;
    uint32_t last = xtimer_now_usec();

    while (remaining > 0) {
        /* handle scheduled callbacks and pending exceptions first */
        if (flags & MP_RIOT_FLAG_SCHED) {
            mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);
        }

        /* every iteration, sleep for the remaining time, but wake up if a
         * callback is scheduled */
        uint32_t now = xtimer_now_usec();
        uint32_t elapsed = now - last;
        last = now;
        if (elapsed >= remaining) {
            break;
        }
        remaining -= elapsed;

        xtimer_t t;
        xtimer_set_timeout_flag(&t, remaining);
        flags = thread_flags_wait_any(MP_RIOT_FLAG_SCHED | MP_RIOT_FLAG_TIMEOUT);
        xtimer_remove(&t);
    }
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    int r = stdio_write(str, len);

    (void)r;
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len)
{
    while (len--) {
        if (*str == '\n') {
            mp_hal_stdout_tx_strn("\r", 1);
        }
        mp_hal_stdout_tx_strn(str++, 1);
    }
}

void mp_hal_stdout_tx_str(const char *str)
{
    mp_hal_stdout_tx_strn(str, strlen(str));
}
