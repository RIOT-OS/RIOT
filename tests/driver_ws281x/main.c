/*
 * Copyright 2019 Marian Buschsieweke
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
 * @brief   Test application for the WS281x RGB LED driver
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "ws281x.h"
#include "ws281x_params.h"
#include "xtimer.h"

static const color_rgb_t rainbow[] = {
    {.r = 0x94, .g = 0x00, .b = 0xd3},
    {.r = 0x4b, .g = 0x00, .b = 0x82},
    {.r = 0x00, .g = 0x00, .b = 0xff},
    {.r = 0x00, .g = 0xff, .b = 0x00},
    {.r = 0xff, .g = 0xff, .b = 0x00},
    {.r = 0xff, .g = 0x7f, .b = 0xd3},
    {.r = 0xff, .g = 0x00, .b = 0x00},
};

#define RAINBOW_LEN     ARRAY_SIZE(rainbow)

int main(void)
{
    ws281x_t dev;
    int retval;

    puts(
        "WS281x Test Application\n"
        "=========================\n"
        "\n"
        "If you see an animated rainbow, the driver works as expected.\n"
        "If the LEDs are flickering, check if the power supply is sufficient\n"
        "(at least 4V). Also: The logic level has to be at least 0.7 * VDD,\n"
        "so 3.3V logic with a 5V power supply is out of spec, but might work\n"
        "OK.\n"
    );

    if (0 != (retval = ws281x_init(&dev, &ws281x_params[0]))) {
        printf("Initialization failed with error code %d\n", retval);
        return retval;
    }

    while (1) {
        unsigned offset = 0;
        puts("\nAnimation: Moving rainbow...");
        xtimer_ticks32_t last_wakeup = xtimer_now();
        for (unsigned i = 0; i < 100; i++) {
            for (uint16_t j = 0; j < dev.params.numof; j++) {
                ws281x_set(&dev, j, rainbow[(j + offset) % RAINBOW_LEN]);
            }
            offset++;
            ws281x_write(&dev);
            xtimer_periodic_wakeup(&last_wakeup, 100 * US_PER_MS);
        }

        puts("\nAnimation: Fading rainbow...");
        last_wakeup = xtimer_now();
        for (unsigned i = 0; i < RAINBOW_LEN; i++) {
            for (unsigned j = 0; j < 255; j++) {
                color_rgb_t col;
                color_rgb_set_brightness(&rainbow[i], &col, j);
                for (uint16_t k = 0; k < dev.params.numof; k++) {
                    ws281x_set(&dev, k, col);
                }
                ws281x_write(&dev);
                xtimer_periodic_wakeup(&last_wakeup, 10 * US_PER_MS);
            }
            for (unsigned j = 255; j > 0; j--) {
                color_rgb_t col;
                color_rgb_set_brightness(&rainbow[i], &col, j);
                for (uint16_t k = 0; k < dev.params.numof; k++) {
                    ws281x_set(&dev, k, col);
                }
                ws281x_write(&dev);
                xtimer_periodic_wakeup(&last_wakeup, 10 * US_PER_MS);
            }
        }

        puts("\nAnimation: 100 rainbows. (You'll need a long chain for this)");
        uint8_t buf[RAINBOW_LEN * WS281X_BYTES_PER_DEVICE];
        for (unsigned i = 0; i < RAINBOW_LEN; i++) {
            ws281x_set_buffer(buf, i, rainbow[i]);
        }

        ws281x_prepare_transmission(&dev);
        for (unsigned i = 0; i < 100; i++) {
            ws281x_write_buffer(&dev, buf, sizeof(buf));
        }
        ws281x_end_transmission(&dev);

        /* wait some time to allow testers to verify the result */
        xtimer_sleep(5);
    }

    return 0;
}
