/*
 * Copyright (C) 2017 Technische Universität Berlin
 *               2017,2018 Freie Universität Berlin
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
 * @brief       Test application for testing priority inheritance when using
 *              nested msg_send_receive calls
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define TICK_LEN            (50UL * US_PER_MS)
#define EXPECTED_RESULT     (-3)

#define T_NUMOF             (3U)
#define MSG_TYPE            (0xabcd)

enum {
    HIGH  = 0,
    MID  = 1,
    LOW = 2,
};

static char _stacks[T_NUMOF][THREAD_STACKSIZE_MAIN];
static kernel_pid_t _pids[T_NUMOF];
static const char *_names[] = { "t_high", " t_mid", " t_low" };

static int _result = 0;
static int _res_addsub = 1;

static inline void _delay(unsigned ticks)
{
    xtimer_usleep(ticks * TICK_LEN);
}

static inline void _event(int num, const char *t_name, const char *msg)
{
    /* record event */
    _result += (_res_addsub * num);
    _res_addsub *= -1;

    printf("Event %2i: %7s - %s\n", num, t_name, msg);
}

static void *t_high(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    m.type = MSG_TYPE;
    m.content.value = (uint32_t)'M';

    _delay(1);

    _event(2, _names[HIGH], "sending msg to t_low (msg_send_receive)");
    msg_send_receive(&m, &rply, _pids[2]);
    _event(6, _names[HIGH], "received reply");

    return NULL;
}

static void *t_mid(void *arg)
{
    (void)arg;

    _delay(2);

    _event(4, _names[MID], "starting infinite loop, potentially starving others");
    while (1) {
        thread_yield_higher();
    }

    return NULL;
}

static void *t_low(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    rply.type = MSG_TYPE;
    rply.content.value = (uint32_t)'m';

    _event(1, _names[LOW], "waiting for incoming message");
    msg_receive(&m);
    _event(3, _names[LOW], "received message and do some quick work");

    _delay(2);

    _event(5, _names[LOW], "sending reply");
    msg_reply(&m, &rply);

    return NULL;
}

static thread_task_func_t _handlers[] = { t_high, t_mid, t_low };

int main(void)
{
    puts("Test for showing priority inversion when using msg_send_receive\n");
    puts("If this tests succeeds, you should see 6 events appearing in order.\n"
         "The expected output should look like this:\n"
         "Event  1:   t_low - waiting for incoming message\n"
         "Event  2:  t_high - sending msg to t_low (msg_send_receive)\n"
         "Event  3:   t_low - received message and do some quick work\n"
         "Event  4:   t_mid - starting infinite loop, potentially starving others\n"
         "Event  5:   t_low - sending reply\n"
         "Event  6:  t_high - received reply\n");

    puts("TEST OUTPUT:");

    /* create threads */
    for (unsigned i = 0; i < T_NUMOF; i++) {
        _pids[i] = thread_create(_stacks[i], sizeof(_stacks[i]),
                                 (THREAD_PRIORITY_MAIN + 1 + i),
                                 THREAD_CREATE_WOUT_YIELD,
                                 _handlers[i], NULL,
                                 _names[i]);
    }

    _delay(4);

    if (_result == EXPECTED_RESULT) {
        puts("\n[SUCCESS]");
    }
    else {
        puts("\n[FAILED]");
    }

    return 0;
}
