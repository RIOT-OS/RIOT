/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the stdio_notify module
 *
 * This demonstrates how the `stdio_notify` module can be used to wait on stdin
 * input without blocking on @ref stdio_read. A notify callback raises a thread
 * flag once data is available, so the main thread can sleep until either new
 * input arrives or a periodic timer fires, and service both events from a
 * single @ref thread_flags_wait_any.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "stdio_base.h"
#include "thread.h"
#include "thread_flags.h"
#include "ztimer.h"

#define FLAG_STDIN          (1U << 0)
#define FLAG_TIMER          (THREAD_FLAG_TIMEOUT)

#define TIMER_INTERVAL_SEC  (5U)

static thread_t *_main_thread;
static ztimer_t _timer;

/**
 * @brief   Invoked in (ISR) context by the stdio backend once data is
 *          available on stdin.
 */
static void _stdin_notify(void *arg)
{
    (void)arg;

    if (_main_thread) {
        thread_flags_set(_main_thread, FLAG_STDIN);
    }
}

int main(void)
{
    _main_thread = thread_get_active();

    /* register the notify callback that wakes us when stdin has data */
    stdio_set_notify(_stdin_notify, NULL);

    /* schedule the periodic timer */
    ztimer_set_timeout_flag(ZTIMER_SEC, &_timer, TIMER_INTERVAL_SEC);

    puts("Type characters to echo them, or wait for the periodic timer.");

    while (1) {
        /* sleep until either stdin has data or the timer fired; this avoids
         * busy-polling stdio_available() and avoids blocking in stdio_read() */
        thread_flags_t flags = thread_flags_wait_any(FLAG_STDIN | FLAG_TIMER);

        if (flags & FLAG_TIMER) {
            /* handle the timer and re-arm it */
            puts("*** Woken up! ***");
            ztimer_set_timeout_flag(ZTIMER_SEC, &_timer, TIMER_INTERVAL_SEC);
        }

        if (flags & FLAG_STDIN) {
            /* drain everything that is currently buffered */
            int available = stdio_available();

            while (available > 0) {
                char buf[16];
                size_t len = (size_t)available < sizeof(buf)
                           ? (size_t)available : sizeof(buf);
                ssize_t res = stdio_read(buf, len);

                if (res <= 0) {
                    break;
                }

                /* echo back to stdio */
                stdio_write(buf, (size_t)res);

                available -= (int)res;
            }
        }
    }

    return 0;
}
