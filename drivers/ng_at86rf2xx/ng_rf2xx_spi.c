/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       Register access functions for AT86RF2XX radio devices
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "ng_at86rf2xx_spi.h"
#include "ng_at86rf2xx_registers.h"
#include "ng_at86rf2xx.h"
#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

void ng_at86rf2xx_reg_write(const ng_at86rf2xx_t *dev, const uint8_t addr, const uint8_t value)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Start the SPI transfer */
    gpio_clear(dev->cs_pin);
    /* write to register */
    spi_transfer_reg(dev->spi, NG_AT86RF2XX_ACCESS_REG | NG_AT86RF2XX_ACCESS_WRITE | addr, value, 0);
    /* End the SPI transfer */
    gpio_set(dev->cs_pin);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
}

uint8_t ng_at86rf2xx_reg_read(const ng_at86rf2xx_t *dev, const uint8_t addr)
{
    char value;

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Start the SPI transfer */
    gpio_clear(dev->cs_pin);
    /* read from register */
    spi_transfer_reg(dev->spi, NG_AT86RF2XX_ACCESS_REG | NG_AT86RF2XX_ACCESS_READ | addr, 0, &value);
    /* End the SPI transfer */
    gpio_set(dev->cs_pin);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return (uint8_t)value;
}

void ng_at86rf2xx_addr_read_fifo(const ng_at86rf2xx_t *dev,
                                 const uint8_t addr,
                                 uint8_t *data,
                                 const radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Start the SPI transfer */
    gpio_clear(dev->cs_pin);
    /* Initiate SRAM access from offset `addr` */
    spi_transfer_reg(dev->spi, NG_AT86RF2XX_ACCESS_SRAM | NG_AT86RF2XX_ACCESS_READ, (char)addr, NULL);
    /* Read a number of bytes from the devices frame buffer */
    spi_transfer_bytes(dev->spi, NULL, (char*)data, length);
    /* End the SPI transfer */
    gpio_set(dev->cs_pin);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
}

void ng_at86rf2xx_addr_write_fifo(const ng_at86rf2xx_t *dev,
                                  const uint8_t addr,
                                  const uint8_t *data,
                                  const radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Start the SPI transfer */
    gpio_clear(dev->cs_pin);
    /* Initiate SRAM access from offset `addr` */
    spi_transfer_reg(dev->spi, NG_AT86RF2XX_ACCESS_SRAM | NG_AT86RF2XX_ACCESS_WRITE, (char)addr, NULL);
    /* Write `length` number of bytes to the device's frame buffer */
    spi_transfer_bytes(dev->spi, (char*)data, NULL, length);
    /* End the SPI transfer */
    gpio_set(dev->cs_pin);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
}

void ng_at86rf2xx_bulk_read_fifo(const ng_at86rf2xx_t *dev,
                                 uint8_t *data,
                                 const radio_packet_length_t length)
{
    ng_at86rf2xx_addr_read_fifo(dev, 0x00, data, length);
}

void ng_at86rf2xx_bulk_write_fifo(const ng_at86rf2xx_t *dev,
                                  const uint8_t *data,
                                  const radio_packet_length_t length)
{
    ng_at86rf2xx_addr_write_fifo(dev, 0x00, data, length);
}

uint8_t ng_at86rf2xx_get_status(const ng_at86rf2xx_t *dev)
{
    return (ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_STATUS)
            & NG_AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}
