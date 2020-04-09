/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "fmt.h"
#include "kernel_defines.h"
#include "msg.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

#define MAIN_MSG_QUEUE_SIZE     (4U)
#define TIMERS_NUMOF            (3U)

msg_t _main_msg_queue[MAIN_MSG_QUEUE_SIZE];
static const uint64_t _timers_offsets[TIMERS_NUMOF] = {
    /* MUST ASCEND */
    1 * US_PER_SEC,
    2 * US_PER_SEC,
    3 * US_PER_SEC,
};

int main(void)
{
    xtimer_t timers[TIMERS_NUMOF];
    msg_t msgs[TIMERS_NUMOF];
    uint64_t start;

    expect(ARRAY_SIZE(_timers_offsets) == TIMERS_NUMOF);
    msg_init_queue(_main_msg_queue, MAIN_MSG_QUEUE_SIZE);
    /* ensure that xtimer_now64() is greater than UINT32_MAX */
    _xtimer_current_time = (2LLU << 32U);
    xtimer_init();
    print_str("Setting ");
    print_u32_dec(TIMERS_NUMOF);
    print_str(" timers:\n");
    for (unsigned i = 0; i < TIMERS_NUMOF; i++) {
        msgs[i].content.value = i;
        print_str(" #");
        print_u32_dec(i);
        print_str(" in ");
        print_u64_dec(_timers_offsets[i]);
        print_str(" usec\n");
    }
    print_str("now=");
    start = xtimer_now64().ticks64;
    print_u64_dec(start);
    print_str("\n");
    expect(start > UINT32_MAX);
    /* set timers after all were printed for better timing */
    for (unsigned i = 0; i < TIMERS_NUMOF; i++) {
        xtimer_set_msg64(&timers[i], _timers_offsets[i], &msgs[i],
                         thread_getpid());
        expect(timers[i].long_start_time > 0);
    }
    while (1) {
        msg_t msg;

        msg_receive(&msg);
        print_str("#");
        print_u32_dec(msg.content.value);
        print_str(":now=");
        print_u64_dec((uint64_t)xtimer_now64().ticks64);
        print_str("\n");
        for (unsigned i = 0; i <= msg.content.value; i++) {
            /* all fired timers expired */
            expect(timers[i].long_start_time == 0);
        }
        for (unsigned i = (msg.content.value + 1); i <= TIMERS_NUMOF; i++) {
            /* upper half of remaing_timers' start_time stays above 0 as it is
             * based on xtimer_now64() during the timer's callback execution */
            expect(timers[i].long_start_time > 0);
        }
    }
    return 0;
}

/** @} */
