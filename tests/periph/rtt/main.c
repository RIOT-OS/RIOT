/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <string.h>
#include <inttypes.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"
#include "periph/rtc_mem.h"

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

#ifdef MODULE_PERIPH_RTC_MEM
static const uint8_t riot_msg_offset = 1;
static const char riot_msg[] = "RIOT";
static void _set_rtc_mem(void)
{
    /* first fill the whole memory */
    uint8_t size = rtc_mem_size();
    while (size--) {
        rtc_mem_write(size, &size, sizeof(size));
    }

    /* write test data */
    rtc_mem_write(riot_msg_offset, riot_msg, sizeof(riot_msg) - 1);
}

static void _get_rtc_mem(void)
{
    char buf[sizeof(riot_msg) - 1];
    rtc_mem_read(riot_msg_offset, buf, sizeof(buf));

    if (memcmp(buf, riot_msg, sizeof(buf))) {
        puts("RTC mem content does not match");
        for (unsigned i = 0; i < sizeof(buf); ++i) {
            printf("%02x - %02x\n", riot_msg[i], buf[i]);
        }
        return;
    }

    uint8_t size = rtc_mem_size();
    while (size--) {
        uint8_t data;

        if (size >= riot_msg_offset &&
            size < riot_msg_offset + sizeof(riot_msg)) {
            continue;
        }

        rtc_mem_read(size, &data, 1);
        if (data != size) {
            puts("RTC mem content does not match");
            printf("%02x: %02x\n", size, data);
        }
    }

    puts("RTC mem OK");
}
#else
static inline void _set_rtc_mem(void) {}
static inline void _get_rtc_mem(void) {}
#endif

int main(void)
{
    puts("\nRIOT RTT low-level driver test");

    puts("RTT configuration:");
    printf("RTT_MAX_VALUE: 0x%08" PRIx32 "\n", (uint32_t)RTT_MAX_VALUE);
    printf("RTT_FREQUENCY: %" PRIu32 "\n\n", (uint32_t)RTT_FREQUENCY);

    puts("Testing the tick conversion (with rounding if RTT_FREQUENCY is not power of 2)");
    for (uint16_t i = 0; i < ARRAY_SIZE(_ticktest); i++) {
        uint32_t sec = RTT_TICKS_TO_SEC(_ticktest[i]);
        uint32_t ticks = RTT_SEC_TO_TICKS(sec);
        printf("Trying to convert %" PRIu32 " to seconds and back\n",
               _ticktest[i]);
        /* RTT_FREQUENCY is not always power of 2 so compare with rounded result */
        if ((ticks != 0) && (ticks != _ticktest[i] / RTT_FREQUENCY * RTT_FREQUENCY)) {
            puts("error: TICK conversion not working as expected\n");
            return 1;
        }
    }
    puts("All ok\n");

    puts("Initializing the RTT driver");
    rtt_init();

    _set_rtc_mem();
    _get_rtc_mem();

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

    _get_rtc_mem();

    puts("Done setting up the RTT, wait for many Hellos");
    return 0;
}
