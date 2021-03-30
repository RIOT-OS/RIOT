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
 * 'Hello' every 5 seconds
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

#define TICKS_TO_WAIT       (5 * RTT_FREQUENCY)

static volatile uint32_t last;

static const uint32_t _ticktest[] = { 1, 256, 65536, 16777216, 2147483648 };

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

    puts("RTT configuration:");
    printf("RTT_MAX_VALUE: 0x%08x\n", (unsigned)RTT_MAX_VALUE);
    printf("RTT_FREQUENCY: %u\n\n", (unsigned)RTT_FREQUENCY);

    puts("Testing the tick conversion");
    for (unsigned i = 0; i < ARRAY_SIZE(_ticktest); i++) {
        uint32_t sec = RTT_TICKS_TO_SEC(_ticktest[i]);
        uint32_t ticks = RTT_SEC_TO_TICKS(sec);
        printf("Trying to convert %" PRIu32 " to seconds and back\n",
               _ticktest[i]);
        /* account for rounding errors... */
        if ((ticks != 0) && (ticks != _ticktest[i])) {
            puts("error: TICK conversion not working as expected\n");
            return 1;
        }
    }
    puts("All ok\n");

    puts("Initializing the RTT driver");
    rtt_init();

    puts("This test will now display 'Hello' every 5 seconds\n");
    uint32_t now = rtt_get_counter();
    printf("RTT now: %" PRIu32 "\n", now);

    if (IS_USED(MODULE_PERIPH_RTT_SET_COUNTER)) {
        puts("Setting RTT timer to 1337");
        rtt_set_counter(1337);
        now = rtt_get_counter();
        printf("RTT now: %" PRIu32 "\n", now);
        if ((now < 1337) || (now > 1337 + (1 * RTT_FREQUENCY + 999) / 1000)) {
            puts("ERROR: rtt_set_counter() failed (off by more than 1 ms)");
            return 1;
        }
        else {
            puts("rtt_set_counter() PASSED");
        }
    }

    last = (now + TICKS_TO_WAIT) & RTT_MAX_VALUE;
    printf("Setting initial alarm to now + 5 s (%" PRIu32 ")\n", last);
    rtt_set_alarm(last, cb, 0);

    uint32_t tmp = rtt_get_alarm();
    if (tmp != last) {
        printf("Error: rtt_get_alarm() not working (expected %" PRIu32 " but got %" PRIu32 ")\n",
               last, tmp);
        return 1;
    }
    else {
        puts("rtt_get_alarm() PASSED");
    }

    puts("Done setting up the RTT, wait for many Hellos");
    return 0;
}
