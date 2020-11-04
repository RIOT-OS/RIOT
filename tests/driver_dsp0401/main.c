/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the DSP0401 4 digit display.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "dsp0401_params.h"
#include "dsp0401.h"
#include "xtimer.h"

#define TEST_DELAY      (2U)               /* 2 seconds delay between each test */
#define WORD_DELAY      (750U)             /* 750 milliseconds delay between each word */
#define SCROLL_DELAY    (200U)             /* 200 milliseconds delay between character shift */
#ifndef LOOPS
#define LOOPS           (3U)               /* Number of display loops before exit */
#endif

int main(void)
{
    dsp0401_t dev;

    puts("Starting DSP0401 test application\n");

    puts("Initializing DSP4010 device.");
    if (dsp0401_init(&dev, &dsp0401_params[0]) != DSP0401_OK) {
        puts("[Error] Cannot initialize DSP0401 display.");
        return 1;
    }

    puts("Initialization successful\n");
    uint8_t loop = 0;
    while (loop < LOOPS) {
        puts("[INFO] Displaying 'THIS IS RIOT'");
        dsp0401_display_text(&dev, (char*)"THIS");
        xtimer_msleep(WORD_DELAY);
        dsp0401_display_text(&dev, (char*)" IS ");
        xtimer_msleep(WORD_DELAY);
        dsp0401_display_text(&dev, (char*)"RIOT");
        xtimer_sleep(TEST_DELAY);

        puts("[INFO] Clearing text!");
        dsp0401_clear_text(&dev);
        xtimer_sleep(TEST_DELAY);

        puts("[INFO] Scrolling 'THIS IS RIOT'");
        dsp0401_scroll_text(&dev, (char*)("THIS IS RIOT"), SCROLL_DELAY);
        xtimer_sleep(TEST_DELAY);
        puts("[INFO] Done\n");
        ++loop;
    }

    puts("SUCCESS");

    return 0;
}
