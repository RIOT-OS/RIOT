/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2018 Josua Arndt
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
 * @brief       xtimer_usleep test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "ztimer.h"
#include "timex.h"

#define RUNS                (5U)
#define SLEEP_TIMES_NUMOF   ARRAY_SIZE(sleep_times)

static const uint32_t sleep_times[] = { 40, 20, 1, 10000, 50000, 10234, 56780,
                                        12122, 98765, 75000, 1000000 };

#define ERROR_US 70

/*
 * To use a pin to probe the sleep times enable the gpio module and define
 * SLEEP_PORT and SLEEP_PIN in the makefile, the port information can be found
 * in the enum in `cpu/include/periph_cpu.h`.
 *
 * FEATURES_REQUIRED += periph_gpio
 * CFLAGS += -DSLEEP_PIN=7
 * CFLAGS += -DSLEEP_PORT=PORT_F
 * */
#ifdef SLEEP_PIN
#include "board.h"
#include "periph/gpio.h"
#endif

int main(void)
{
    uint32_t start_test, testtime;

#ifdef SLEEP_PIN
    printf("Debug port 0x%02x pin %d\n", SLEEP_PORT, SLEEP_PIN);
    gpio_t sleep_pin = GPIO_PIN(SLEEP_PORT, SLEEP_PIN);
    gpio_init(sleep_pin, GPIO_OUT);
#endif

    printf("Running test %u times with %u distinct sleep times\n", RUNS,
           (unsigned)SLEEP_TIMES_NUMOF);
    start_test = ztimer_now(ZTIMER_USEC);
    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0;
             n < ARRAY_SIZE(sleep_times);
             n++) {

            uint32_t start_sleep, diff;
            int32_t err;

            start_sleep = ztimer_now(ZTIMER_USEC);

#ifdef SLEEP_PIN
            gpio_set(sleep_pin);
#endif
            ztimer_sleep(ZTIMER_USEC, sleep_times[n]);
#ifdef SLEEP_PIN
            gpio_clear(sleep_pin);
#endif

            diff = ztimer_now(ZTIMER_USEC) - start_sleep;

            err = diff - sleep_times[n];

            printf("Slept for %" PRIu32 " us (expected: %" PRIu32 " us) "
                   "Offset: %" PRIi32 " us\n", diff, sleep_times[n], err);
        }
    }
    testtime = ztimer_now(ZTIMER_USEC) - start_test;
    printf("Test ran for %" PRIu32 " us\n", testtime);

    return 0;
}
