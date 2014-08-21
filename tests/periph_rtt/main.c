/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level Real Time Timer drivers
 *
 * This test will initialize the real-time timer and trigger an alarm printing
 * 'Hello' every 10 seconds
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"

/* only compile this test if a RTT device is defined */
#if RTT_NUMOF

#define TICKS_TO_WAIT       (10 * RTT_FREQUENCY)

void cb(void *arg)
{
    (void)arg;
    uint32_t now;

    now = rtt_get_counter() + TICKS_TO_WAIT;
    now = (now > RTT_MAX_VALUE) ? now - RTT_MAX_VALUE : now;
    rtt_set_alarm(now, cb, 0);
    puts("Hello");
}


int main(void)
{
    puts("\nRIOT RTT low-level driver test");
    puts("This test will display 'Hello' every 10 seconds\n");

    puts("Initializing the RTT driver");
    rtt_init();

    puts("Setting initial alarm");
    rtt_set_alarm(TICKS_TO_WAIT, cb, 0);

    puts("Done setting up the RTT, wait for many Hellos");
    return 0;
}

#else

int main(void)
{
    puts("\nRIOT RTT low-level driver test");
    puts("There are no RTT devices defined for this board!");

    return 0;
}

#endif /* RTT_NUMOF */
