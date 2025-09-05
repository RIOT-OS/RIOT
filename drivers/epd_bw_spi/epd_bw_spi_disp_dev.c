/*
 * SPDX-FileCopyrightText: 2022 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_epd_bw_spi
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "epd_bw_spi.h"
#include "epd_bw_spi_disp_dev.h"

#ifndef EPD_BW_SPI_DISP_COLOR_DEPTH
#define EPD_BW_SPI_DISP_COLOR_DEPTH (1U)
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static void _epd_bw_spi_map(const disp_dev_t *disp_dev, const disp_dev_area_t *area,
                            const uint16_t *color)
{
    /* Convert the area to non-inclusive coordinates */
    uint8_t x1 = area->x1;
    uint8_t x2 = area->x2 + 1;
    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2 + 1;

    epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;
    uint16_t size = ((x2 - x1) >> 3) * (y2 - y1);

    DEBUG("[epd_bw_spi] map: (%d,%d)-(%d,%d) (%u bytes) count %d\n",
          x1, y1, x2, y2, size, dev->partial_refresh_count);

    /* Wake the device from sleep */
    epd_bw_spi_wake(dev);

    /* Initialize and update display */
    epd_bw_spi_init_auto(dev);
    epd_bw_spi_activate(dev);
    epd_bw_spi_set_area(dev, x1, x2, y1, y2);
    epd_bw_spi_write_buffer(dev, (uint8_t *)color, size);
    epd_bw_spi_update_auto(dev);

    /* Put the display to sleep */
    epd_bw_spi_deactivate(dev);
    if (gpio_is_valid(dev->params.rst_pin)) {
        epd_bw_spi_sleep(dev);
    }
}

static uint16_t _epd_bw_spi_height(const disp_dev_t *disp_dev)
{
    epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;

    assert(dev);

    return dev->params.size_y;
}

static uint16_t _epd_bw_spi_width(const disp_dev_t *disp_dev)
{
    const epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;

    assert(dev);

    return dev->params.size_x;
}

static uint8_t _epd_bw_spi_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return EPD_BW_SPI_DISP_COLOR_DEPTH;
}

/* This function does nothing, as it is not supported by this kind of display */
static void _epd_bw_spi_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    (void)disp_dev;
    (void)invert;
}

const disp_dev_driver_t epd_bw_spi_disp_dev_driver = {
    .map = _epd_bw_spi_map,
    .height = _epd_bw_spi_height,
    .width = _epd_bw_spi_width,
    .color_depth = _epd_bw_spi_color_depth,
    .set_invert = _epd_bw_spi_set_invert,
};
