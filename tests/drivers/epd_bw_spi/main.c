/*
 * SPDX-FileCopyrightText: 2019 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the black/white e-paper SPI display driver.
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#include "epd_bw_spi.h"
#include "epd_bw_spi_params.h"

#include "pictures.h"

const uint8_t riot_32_width = 32;
const uint16_t riot_32_height = sizeof(riot_logo_32) / sizeof(riot_logo_32[0]);

/* Draw the large RIOT logo with a full refresh */
void draw_riot(epd_bw_spi_t *dev)
{
    epd_bw_spi_wake(dev);
    epd_bw_spi_init_full(dev);
    epd_bw_spi_activate(dev);
    epd_bw_spi_write_buffer(dev, (uint8_t *)riot_icon_200,
                            sizeof riot_icon_200);
    epd_bw_spi_update_full(dev);
    epd_bw_spi_deactivate(dev);
    if (dev->params.rst_pin != GPIO_UNDEF) {
        epd_bw_spi_sleep(dev);
    }
}

/* Draw a small RIOT logo with a partial refresh */
void draw_small_riot(epd_bw_spi_t *dev, uint8_t x, uint16_t y)
{
    epd_bw_spi_wake(dev);
    epd_bw_spi_init_part(dev);
    epd_bw_spi_activate(dev);
    epd_bw_spi_set_area(dev, x, x + riot_32_width, y, y + riot_32_height);
    epd_bw_spi_write_buffer(dev, (uint8_t *)riot_logo_32, sizeof riot_logo_32);
    epd_bw_spi_update_part(dev);
    epd_bw_spi_deactivate(dev);
    if (dev->params.rst_pin != GPIO_UNDEF) {
        epd_bw_spi_sleep(dev);
    }
}

int main(void)
{
    epd_bw_spi_t dev;
    int init = epd_bw_spi_init(&dev, epd_bw_spi_params);

    if (init != 0) {
        printf("EPD_BW_SPI INIT FAILED: %i\n", init);
        return 1;
    }

    while (1) {
        /* Set both RAM buffers to the RIOT logo */
        draw_riot(&dev);
        draw_riot(&dev);

        /* Draw small RIOT logos on the display */
        for (uint16_t y = 0; y < dev.params.size_y; y += riot_32_height) {
            for (uint16_t x = 0; x < dev.params.size_x; x += 32) {
                draw_small_riot(&dev, x, y);

                ztimer_sleep(ZTIMER_MSEC, 2500);
            }
        }
    }

    return 0;
}
