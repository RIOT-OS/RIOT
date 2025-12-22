/*
 * SPDX-FileCopyrightText: 2015-2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2013 INRIA
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       ztimer64_msg test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "ztimer64.h"
#include "thread.h"
#include "msg.h"
#include "timex.h"

#include "test_utils/expect.h"

#ifdef MODULE_ZTIMER64_SEC
#define ZTIMER64 ZTIMER64_SEC
#define TICKS_PER_SEC 1
#elif MODULE_ZTIMER64_MSEC
#define ZTIMER64 ZTIMER64_MSEC
#define TICKS_PER_SEC MS_PER_SEC
#else
#define ZTIMER64 ZTIMER64_USEC
#define TICKS_PER_SEC US_PER_SEC
#endif

char timer_stack[THREAD_STACKSIZE_DEFAULT];
char timer_stack_local[THREAD_STACKSIZE_DEFAULT];

struct timer_msg {
    ztimer64_t timer;
    uint64_t interval;
    char *text;
    msg_t msg;
};

struct timer_msg msg_a = { .interval = (2 * TICKS_PER_SEC),
                           .text = "Hello World" };
struct timer_msg msg_b = { .interval = (5 * TICKS_PER_SEC),
                           .text = "This is a Test" };

void *timer_thread(void *arg)
{
    (void)arg;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    /* The queue is required to avoid loss of a 2nd message, when the 1st is
     * still processed. The timing ensures that at most 1 message is queued.
     */
    msg_t msgq[1];

    msg_init_queue(msgq, 1);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = m.content.ptr;
        uint64_t now = ztimer64_now(ZTIMER64);
        /* casts are needed to solve for sometimes TICKS_PER_SEC being UL
         * result of / and % of uint32_t will always fit into uint32_t
         */
        printf(
            "now=%" PRIu32 ":%" PRIu32 " -> every %" PRIu32 ".%" PRIu32 "s: %s\n",
            (uint32_t)(now / TICKS_PER_SEC),
            (uint32_t)(now % TICKS_PER_SEC),
            (uint32_t)(tmsg->interval / TICKS_PER_SEC),
            (uint32_t)(tmsg->interval % TICKS_PER_SEC),
            tmsg->text);

        tmsg->msg.type = 12345;
        tmsg->msg.content.ptr = tmsg;
        ztimer64_set_msg(ZTIMER64, &tmsg->timer, tmsg->interval, &tmsg->msg,
                         thread_getpid());
    }
}

static void _timestamp_to_gmt_civil(struct tm *_tm, uint32_t epoch)
{
    uint32_t s = epoch % 86400;

    epoch /= 86400;
    uint32_t h = s / 3600;
    uint32_t m = s / 60 % 60;

    s = s % 60;
    uint32_t x = (epoch * 4 + 102032) / 146097 + 15;
    uint32_t b = epoch + 2442113 + x - (x / 4);
    uint32_t c = (b * 20 - 2442) / 7305;
    uint32_t d = b - 365 * c - c / 4;
    uint32_t e = d * 1000 / 30601;
    uint32_t f = d - e * 30 - e * 601 / 1000;

    if (e < 14) {
        struct tm tmp =
        { .tm_year = c - 4716 - 1900, .tm_mon = e - 1, .tm_mday = f,
          .tm_hour = h, .tm_min = m, .tm_sec = s };
        *_tm = tmp;
    }
    else {
        struct tm tmp =
        { .tm_year = c - 4715 - 1900, .tm_mon = e - 13, .tm_mday = f,
          .tm_hour = h, .tm_min = m, .tm_sec = s };
        *_tm = tmp;
    }

    /* struct tm counts months starting from 0 */
    _tm->tm_mon -= 1;
}

void *timer_thread_local(void *arg)
{
    (void)arg;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);

        struct tm tmp;
        uint64_t now = ztimer64_now(ZTIMER64);
        _timestamp_to_gmt_civil(&tmp, now / 1000000);
        uint32_t sec = tmp.tm_sec;
        uint32_t min = tmp.tm_min;
        uint32_t hr = tmp.tm_hour;
        printf("%02i.%02i.%04i %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 "\n", 
                tmp.tm_mday, tmp.tm_mon, tmp.tm_year +  1900,
                hr, min, sec);
    }
}

int main(void)
{
    msg_t m;
    kernel_pid_t pid = thread_create(
        timer_stack,
        sizeof(timer_stack),
        THREAD_PRIORITY_MAIN - 1,
        0,
        timer_thread,
        NULL,
        "timer");

    expect(pid_is_valid(pid));

    puts("sending 1st msg");
    m.content.ptr = &msg_a;
    msg_try_send(&m, pid);

    puts("sending 2nd msg");
    m.content.ptr = &msg_b;
    msg_try_send(&m, pid);

    kernel_pid_t pid2 = thread_create(
        timer_stack_local,
        sizeof(timer_stack_local),
        THREAD_PRIORITY_MAIN - 1,
        0,
        timer_thread_local,
        NULL,
        "timer local");

    expect(pid_is_valid(pid2));

    while (1) {
        ztimer64_sleep(ZTIMER64, 1LLU * TICKS_PER_SEC);
        msg_try_send(&m, pid2);
    }
}
