/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_epd_bw_spi
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "epd_bw_spi.h"
#include "epd_bw_spi_disp_dev.h"

#ifndef EPD_BW_SPI_DISP_COLOR_DEPTH
#define EPD_BW_SPI_DISP_COLOR_DEPTH (1U)
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _epd_bw_spi_map_full(epd_bw_spi_t *dev,
                                 uint16_t x1, uint16_t x2,
                                 uint16_t y1, uint16_t y2,
                                 const uint16_t *color)
{
    uint16_t size = ((x2 - x1) >> 3) * (y2 - y1);

    epd_bw_spi_init_full(dev);
    epd_bw_spi_activate(dev);
    epd_bw_spi_set_area(dev, x1, x2, y1, y2);
    epd_bw_spi_write_buffer(dev,  (uint8_t *)color, size);
    epd_bw_spi_update_full(dev);
}

static void _epd_bw_spi_map_part(epd_bw_spi_t *dev,
                                 uint16_t x1, uint16_t x2,
                                 uint16_t y1, uint16_t y2,
                                 const uint16_t *color)
{
    uint16_t size = ((x2 - x1) >> 3) * (y2 - y1);

    DEBUG("[epd_bw_spi] map part (%d,%d)-(%d,%d) (%u bytes)\n",
          x1, y1, x2, y2, size);
    epd_bw_spi_init_part(dev);
    epd_bw_spi_activate(dev);
    epd_bw_spi_set_area(dev, x1, x2, y1, y2);
    epd_bw_spi_write_buffer(dev, (uint8_t *)color, size);
    epd_bw_spi_update_part(dev);
}

static void _epd_bw_spi_map(disp_dev_t *disp_dev, uint16_t x1, uint16_t x2,
                            uint16_t y1, uint16_t y2, const uint16_t *color)
{
    epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;

    /* Wake the device from sleep */
    epd_bw_spi_wake(dev);

    /* Update the display */
    if (dev->refresh_partial) {
        _epd_bw_spi_map_part(dev, x1, x2, y1, y2, color);
    }
    else {
        _epd_bw_spi_map_full(dev, x1, x2, y1, y2, color);
    }

    /* Put the display to sleep */
    epd_bw_spi_deactivate(dev);
    if (dev->params.rst_pin != GPIO_UNDEF) {
        epd_bw_spi_sleep(dev);
    }
}

static uint16_t _epd_bw_spi_height(disp_dev_t *disp_dev)
{
    epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;

    assert(dev);

    return dev->size_y;
}

static uint16_t _epd_bw_spi_width(disp_dev_t *disp_dev)
{
    const epd_bw_spi_t *dev = (epd_bw_spi_t *)disp_dev;

    assert(dev);

    return dev->size_x;
}

static uint8_t _epd_bw_spi_color_depth(disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return EPD_BW_SPI_DISP_COLOR_DEPTH;
}

static void _epd_bw_spi_set_invert(disp_dev_t *disp_dev, bool invert)
{
    (void)disp_dev;
    (void)invert;
    assert(false);
}

const disp_dev_driver_t epd_bw_spi_disp_dev_driver = {
    .map = _epd_bw_spi_map,
    .height = _epd_bw_spi_height,
    .width = _epd_bw_spi_width,
    .color_depth = _epd_bw_spi_color_depth,
    .set_invert = _epd_bw_spi_set_invert,
};
