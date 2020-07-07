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

#include "xtimer.h"

#include "disp_dev.h"

#include "epd_bw_spi.h"
#include "epd_bw_spi_disp_dev.h"
#include "epd_bw_spi_pictures.h"
#include "epd_bw_spi_params.h"

const uint16_t riot_32_height = sizeof(riot_logo_32)/sizeof(riot_logo_32[0]);

int main(void)
{
    epd_bw_spi_t epd;
    int init = epd_bw_spi_init(&epd, epd_bw_spi_params, epd_bw_spi_controller,
                               DISPLAY_X, DISPLAY_Y, EPD_BW_SPI_Y_INC_X_INC);
    if (init != 0) {
        printf("EPD_BW_SPI INIT FAILED: %i\n", init);
        return 1;
    }

    disp_dev_t *dev = (disp_dev_t *)&epd;
    dev->driver = &epd_bw_spi_disp_dev_driver;

    xtimer_ticks32_t last_wakeup = xtimer_now();
    while (1) {
        /* Use full refreshes for the full-size logo */
        epd.refresh_partial = false;

        /* Set both RAM buffers to the RIOT logo */
        disp_dev_map(dev, 0, 200, 0, 200, (uint16_t*)riot_icon_200);
        disp_dev_map(dev, 0, 200, 0, 200, (uint16_t*)riot_icon_200);
        xtimer_periodic_wakeup(&last_wakeup, 5 * US_PER_SEC);

        /* Use partial refreshes for the small logo */
        /* cppcheck-suppress redundantAssignment
         * (reason: this variable is checked in _epd_bw_spi_map by casting
         *  the disp_dev_t back to epd_bw_spi_t) */
        epd.refresh_partial = true;

        /* Draw small RIOT logos on the display */
        for(int y = 0; y < disp_dev_width(dev); y+=riot_32_height) {
            for(int x = 0; x < disp_dev_height(dev); x+=32) {
                disp_dev_map(dev, x, x+32, y, y+riot_32_height, (uint16_t*)riot_logo_32);
                xtimer_periodic_wakeup(&last_wakeup, 2500 * US_PER_MS);
            }
        }
    }

    return 0;
}
