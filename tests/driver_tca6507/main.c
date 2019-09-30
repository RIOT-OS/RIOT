/*
 * Copyright (C) 2019 Noel Le Moult <noel.lemoult@dfxlab.fr>
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the TCA6507 LED driver.
 * @author      Noel Le Moult <noel.lemoult@dfxlab.fr>
 * @author      William MARTIN <william.martin@power-lan.com>
 * @file
 */

#include <stdio.h>

#include "xtimer.h"
#include "tca6507.h"
#include "tca6507_params.h"

#define CHECK(rc) \
    if (rc == TCA6507_OK) { \
        puts("[OK]"); \
    } else { \
        puts("[Failed]"); \
        return -1; \
    }

int main(void)
{
    int rc;
    tca6507_t dev;

    /* Init TCA6507 context */
    puts("TCA6507 LED Driver: test application");
    rc = tca6507_init(&dev, &tca6507_params[0]);
    CHECK(rc);

    /* Shutdown all leds */
    puts("TCA6507 LED Driver: Clear all leds");
    rc = tca6507_clear_all(&dev);
    CHECK(rc);

    /* Configure Brightness */
    puts("TCA6507 LED Driver: Configure Bank 0 at 20%%");
    rc = tca6507_intensity(&dev, TCA6507_BRIGHTNESS_20_PCENT, TCA6507_BANK0);
    CHECK(rc);

    puts("TCA6507 LED Driver: Configure Bank 1 at 100%%");
    rc = tca6507_intensity(&dev, TCA6507_BRIGHTNESS_100_PCENT, TCA6507_BANK1);
    CHECK(rc);

    /* Configure Heartbeat like fadding on bank 0 */
    puts("TCA6507 LED Driver: Configure fade on time at 64ms");
    rc = tca6507_fade_on_time(&dev, TCA6507_TIME_64_MS, TCA6507_BANK0);
    CHECK(rc);

    puts("TCA6507 LED Driver: Configure fade off time at 64ms");
    rc = tca6507_fade_off_time(&dev, TCA6507_TIME_64_MS, TCA6507_BANK0);
    CHECK(rc);

    puts("TCA6507 LED Driver: Configure fully on time at 128ms");
    rc = tca6507_fully_on_time(&dev, TCA6507_TIME_128_MS, TCA6507_BANK0);
    CHECK(rc);

    puts("TCA6507 LED Driver: Configure first fully off time at 256ms");
    rc = tca6507_first_fully_off_time(&dev, TCA6507_TIME_256_MS, TCA6507_BANK0);
    CHECK(rc);

    puts("TCA6507 LED Driver: Configure second fully off time at 256ms");
    rc =
        tca6507_second_fully_off_time(&dev, TCA6507_TIME_1024_MS,
                                      TCA6507_BANK0);
    CHECK(rc);

    /* Start the heartbeat */
    puts("TCA6507 LED Driver: Blink on led 1 at 20%% (i.e. Bank 0)");
    rc = tca6507_blink_led(&dev, 0, TCA6507_BANK0);
    CHECK(rc);

    /* Manual led tests */
    xtimer_usleep(1 * US_PER_SEC);
    for (int i = 0; i < 5; i++) {
        puts("TCA6507 LED Driver: Toggle Bank 1 for led 1");
        rc = tca6507_toggle_led(&dev, 1, TCA6507_BANK1);
        CHECK(rc);

        xtimer_usleep(250 * MS_PER_SEC);
        puts("TCA6507 LED Driver: Toggle Bank 1 for led 2");
        rc = tca6507_toggle_led(&dev, 2, TCA6507_BANK1);
        CHECK(rc);

        xtimer_usleep(1 * US_PER_SEC);
        puts("TCA6507 LED Driver: Toggle Bank 1 for led 2");
        rc = tca6507_toggle_led(&dev, 2, TCA6507_BANK1);
        CHECK(rc);

        xtimer_usleep(250 * MS_PER_SEC);
        puts("TCA6507 LED Driver: Toggle Bank 1 for led 1");
        rc = tca6507_toggle_led(&dev, 1, TCA6507_BANK1);
        CHECK(rc);

        xtimer_usleep(250 * MS_PER_SEC);
    }

    /* Shutdown all leds */
    puts("TCA6507 LED Driver: Clear all leds");
    rc = tca6507_clear_all(&dev);
    CHECK(rc);

    puts("TCA6507 LED Driver: End\n");
    return 0;
}
