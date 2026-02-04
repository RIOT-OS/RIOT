/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "timex.h"
#include "ztimer.h"

#define TEST_DELAY_US   (2U * US_PER_SEC)  /* 2 seconds delay between each test */
#define WORD_DELAY_US   (750U * US_PER_MS) /* 750 milliseconds delay between each word */
#define SCROLL_DELAY_MS (200U)             /* 200 milliseconds delay between character shift */
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
        ztimer_sleep(ZTIMER_USEC, WORD_DELAY_US);
        dsp0401_display_text(&dev, (char*)" IS ");
        ztimer_sleep(ZTIMER_USEC, WORD_DELAY_US);
        dsp0401_display_text(&dev, (char*)"RIOT");
        ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);

        puts("[INFO] Clearing text!");
        dsp0401_clear_text(&dev);
        ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);

        puts("[INFO] Scrolling 'THIS IS RIOT'");
        dsp0401_scroll_text(&dev, (char*)("THIS IS RIOT"), SCROLL_DELAY_MS);
        ztimer_sleep(ZTIMER_USEC, TEST_DELAY_US);
        puts("[INFO] Done\n");
        ++loop;
    }

    puts("SUCCESS");

    return 0;
}
