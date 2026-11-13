/*
 * SPDX-FileCopyrightText: 2026 Kerry Lynn <kerlyn2001@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for non-blocking stdio_rtt output
 *
 * With no RTT host reading the up buffer, a non-blocking write to stdio_rtt
 * drops what does not fit. A caller that loops until all of its output is
 * written, such as write_all() in pkg/mpaland-printf, must still make
 * progress; otherwise it spins forever and starves every thread of the same
 * or lower priority.
 *
 * The main thread writes a numbered line every 100 ms with such a loop, and a
 * second thread of the same priority toggles LED0 every 250 ms. Without an
 * RTT host the up buffer fills after a few lines. If stdio_write() then keeps
 * returning 0, main never yields and LED0 stops.
 *
 * @author      Kerry Lynn <kerlyn2001@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "led.h"
#include "stdio_base.h"
#include "thread.h"
#include "ztimer.h"

static char _blink_stack[THREAD_STACKSIZE_DEFAULT];
static const char _payload[] =
    "0123456789012345678901234567890123456789012345678901234567890123456789012345678";

static void *_blink(void *arg)
{
    (void)arg;

    while (1) {
        LED0_TOGGLE;
        ztimer_sleep(ZTIMER_MSEC, 250);
    }

    return NULL;
}

/* The same loop as write_all() in pkg/mpaland-printf. */
static void _write_all(const char *buf, size_t len)
{
    const char *end = buf + len;

    while (buf < end) {
        ssize_t res = stdio_write(buf, end - buf);
        if (res < 0) {
            return;
        }
        buf += res;
    }
}

int main(void)
{
    /* Same priority as main; RIOT does not time-slice between them. */
    thread_create(_blink_stack, sizeof(_blink_stack), THREAD_PRIORITY_MAIN, 0, _blink, NULL,
                  "blink");

    for (unsigned n = 0;; n++) {
        char line[100];
        int len = snprintf(line, sizeof(line), "line %5u: %s\n", n, _payload);
        _write_all(line, len);
        ztimer_sleep(ZTIMER_MSEC, 100);
    }

    return 0;
}
