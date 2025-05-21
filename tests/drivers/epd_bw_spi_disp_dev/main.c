/*
 * Copyright (C) 2019 Silke Hofstra
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
 * @brief       Test application for the black/white e-paper SPI display driver.
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @}
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ztimer.h"

#include "disp_dev.h"

#include "epd_bw_spi.h"
#include "epd_bw_spi_disp_dev.h"
#include "epd_bw_spi_params.h"

#include "pictures.h"

const uint8_t riot_32_width = 32;
const uint16_t riot_32_height = sizeof(riot_logo_32) / sizeof(riot_logo_32[0]);
const uint8_t riot_200_width = 200;
const uint8_t riot_200_height = 200;

int main(void)
{
    epd_bw_spi_t epd;

    int init = epd_bw_spi_init(&epd, epd_bw_spi_params);

    if (init != 0) {
        printf("EPD_BW_SPI INIT FAILED: %i\n", init);
        return 1;
    }

    disp_dev_t *dev = (disp_dev_t *)&epd;
    disp_dev_area_t large_area = {
        .x1 = 0,
        .x2 = riot_200_width - 1,
        .y1 = 0,
        .y2 = riot_200_width - 1,
    };

    dev->driver = &epd_bw_spi_disp_dev_driver;

    while (1) {
        /* Set both RAM buffers to the RIOT logo */
        disp_dev_map(dev, &large_area, (void *)riot_icon_200);
        disp_dev_map(dev, &large_area, (void *)riot_icon_200);

        /* Draw small RIOT logos on the display */
        for (uint16_t y = 0; y < disp_dev_width(dev); y += riot_32_height) {
            for (uint16_t x = 0; x < disp_dev_height(dev); x += 32) {
                disp_dev_area_t small_area = {
                    .x1 = x,
                    .x2 = x + riot_32_width - 1,
                    .y1 = y,
                    .y2 = y + riot_32_height - 1,
                };

                disp_dev_map(dev, &small_area, (void *)riot_logo_32);

                ztimer_sleep(ZTIMER_MSEC, 2500);
            }
        }
    }

    return 0;
}
