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
#include <inttypes.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"

#define TICKS_TO_WAIT       (10 * RTT_FREQUENCY)

static volatile uint32_t last;

void cb(void *arg)
{
    (void)arg;

    last += TICKS_TO_WAIT;
    last &= RTT_MAX_VALUE;
    rtt_set_alarm(last, cb, 0);

    puts("Hello");
}

int main(void)
{
    puts("\nRIOT RTT low-level driver test");
    puts("This test will display 'Hello' every 10 seconds\n");

    puts("Initializing the RTT driver");
    rtt_init();

    uint32_t now = rtt_get_counter();
    printf("RTT now: %" PRIu32 "\n", now);

    last = (now + TICKS_TO_WAIT) & RTT_MAX_VALUE;
    printf("Setting initial alarm to now + 10 s (%" PRIu32 ")\n", last);
    rtt_set_alarm(last, cb, 0);

    puts("Done setting up the RTT, wait for many Hellos");
    return 0;
}
