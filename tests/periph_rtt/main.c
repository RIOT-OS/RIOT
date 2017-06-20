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

    puts("Setting initial alarm");
    last = TICKS_TO_WAIT;
    rtt_set_alarm(TICKS_TO_WAIT, cb, 0);

    puts("Done setting up the RTT, wait for many Hellos");
    return 0;
}
