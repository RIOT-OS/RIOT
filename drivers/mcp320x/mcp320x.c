/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp320x MCP3204/MCP3208 12-Bit A/D Converter
 * @ingroup     drivers_sensors
 * @brief       Driver for the Microchip MCP320x ADC.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MCP320x ADC driver.
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph/gpio.h"
#include "periph/spi.h"

#include "mcp320x.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define SPI_SPEED                  SPI_SPEED_5MHZ

/**
 * @brief enum representation of the available modes
 */
typedef enum {
    single = 0x2,
    diff = 0x00
} _mcp320x_mt_t;

int mcp320x_init(mcp320x_t *dev, spi_t spi, gpio_t cs, mcp320x_chip_t type)
{
    /* fill the device descriptor */
    dev->spi = spi;
    dev->cs = cs;
    dev->initialized = true;
    dev->type = type;


    /* initialize the chip select line */
    gpio_init(cs, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(cs);

    /* initialize the SPI peripheral */
    return spi_init_master(spi, SPI_CONF_FIRST_RISING, SPI_SPEED);
}

static int _mcp320x_read(mcp320x_t *dev, uint8_t type, uint16_t channel)
{
    uint8_t packet[3] = {
        0x4 | (uint8_t)type | ((channel & 0x04) ? 1 : 0),
        (uint8_t)((channel << 6) & 0xff),
        0x00
    };

    /* acquire exclusive bus access */
    spi_acquire(dev->spi);

    /* select the CS of the MCP320x */
    gpio_clear(dev->cs);

    /* write command and address */
    int status = spi_transfer_bytes(dev->spi, (char *)packet, (char *)packet, sizeof(packet));
    if (status < 0) {
        return status;
    }

    /* release the CS of the MCP320x */
    gpio_set(dev->cs);

    /* release exclusive bus access */
    spi_release(dev->spi);

    return ((uint16_t)(packet[1] & 0x0f)) << 8 | packet[2];
}

int mcp320x_read_single(mcp320x_t *dev, int channel)
{
    /* validate arguments */
    if (!dev || channel >= (int)dev->type) {
        return -1;
    }

    /* read a single ADC channel */
    return _mcp320x_read(dev, single, channel);
}

int mcp320x_read_differential(mcp320x_t *dev, mcp320x_diff_mode_t mode)
{
    /* validate arguments */
    if (!dev || mode >= (int)dev->type) {
        return -1;
    }

    /* make an differential ADC readout in the given mode */
    return _mcp320x_read(dev, diff, mode);
}
