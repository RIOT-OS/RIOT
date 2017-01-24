/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       xtimer_msg test application
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"


/* some internally used msg types */
#define MSG_LONG                (0xcafe)
#define MSG_MID                 (0xe5e1)
#define MSG_TICK                (0xaffe)

/* define sleep and timeout intervals */
#define MIN_TO_USEC(min)        (60UL * min * US_PER_SEC)
#define INT_LONG_MSG            (MIN_TO_USEC(14))
#define INT_LONG_SLEEP          (MIN_TO_USEC(18))
#define INT_MID_MSG             (MIN_TO_USEC(3))
#define INT_MID_SLEEP           (MIN_TO_USEC(5))
#define INT_SHORT               (50UL * US_PER_MS)

/* and some timeout conditions */
#define SHORT_MIN_REACHED       (60 * 20)    /* 60 * 20 * 50ms = 1min */

/* configure the print threads message queue */
#define MSG_Q_SIZE              (8U)
static msg_t mq[MSG_Q_SIZE];

/* allocate some stacks */
static char long_stack[THREAD_STACKSIZE_MAIN];
static char mid_stack[THREAD_STACKSIZE_MAIN];
static char short_stack[THREAD_STACKSIZE_MAIN];

/* the main threads PID */
static kernel_pid_t print_pid;

/* allocate timer structs for mid- and long-term timers */
static xtimer_t long_timer;
static xtimer_t mid_timer;

/* and some software counters */
static int long_msg_ticks = 0;
static int long_sleep_ticks = 0;
static int mid_msg_ticks = 0;
static int mid_sleep_ticks = 0;
static int short_ticks = 0;


void *long_sleep(void *arg)
{
    (void) arg;

    while (1) {
        printf("sleep -- 18min -- %i ticks since\n", long_sleep_ticks);
        long_sleep_ticks = 0;

        xtimer_usleep(INT_LONG_SLEEP);
    }

    return NULL;
}

void *mid_sleep(void *arg)
{
    (void) arg;

    while (1) {
        printf("sleep -- 5min  -- %i ticks since\n", mid_sleep_ticks);
        mid_sleep_ticks = 0;

        xtimer_usleep(INT_MID_SLEEP);
    }

    return NULL;
}

void *ticker(void *arg)
{
    (void)arg;
    xtimer_ticks32_t base = xtimer_now();

    while (1) {
        ++short_ticks;

        if (short_ticks == SHORT_MIN_REACHED) {
            short_ticks = 0;
            ++long_msg_ticks;
            ++long_sleep_ticks;
            ++mid_msg_ticks;
            ++mid_sleep_ticks;

            msg_t msg;
            msg.type = MSG_TICK;
            msg_send(&msg, print_pid);
        }

        xtimer_periodic_wakeup(&base, INT_SHORT);
    }

    return NULL;
}

int main(void)
{
    msg_t msg_mid, msg_long, msg;

    puts("xtimer long-term test");
    puts("Refer to the README to get information on the expected output.");

    /* save the main threads PID */
    print_pid = thread_getpid();

    /* initialize the message queue */
    msg_init_queue(mq, MSG_Q_SIZE);

    /* create the other threads */
    thread_create(long_stack, sizeof(long_stack), THREAD_PRIORITY_MAIN - 1,
                  0, long_sleep, NULL, "long_sleep");
    thread_create(mid_stack, sizeof(mid_stack), THREAD_PRIORITY_MAIN - 2,
                  0, mid_sleep, NULL, "mid_sleep");
    thread_create(short_stack, sizeof(short_stack), THREAD_PRIORITY_MAIN - 3,
                  0, ticker, NULL, "ticks");

    /* initiate the mid- and long-term messages */
    msg_long.type = MSG_LONG;
    xtimer_set_msg(&long_timer, INT_LONG_MSG, &msg_long, print_pid);
    msg_mid.type = MSG_MID;
    xtimer_set_msg(&mid_timer, INT_MID_MSG, &msg_mid, print_pid);

    /* watch for incoming messages */
    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case MSG_LONG:
                printf("msg   -- 14min -- %i ticks since\n", long_msg_ticks);
                long_msg_ticks = 0;
                xtimer_set_msg(&long_timer, INT_LONG_MSG, &msg_long, print_pid);
                break;

            case MSG_MID:
                printf("msg   -- 3min  -- %i ticks since\n", mid_msg_ticks);
                mid_msg_ticks = 0;
                xtimer_set_msg(&mid_timer, INT_MID_MSG, &msg_mid, print_pid);
                break;

            case MSG_TICK:
                puts("TICK  -- 1min");
                break;

            default:
                break;
        }
    }

    return 0;
}
