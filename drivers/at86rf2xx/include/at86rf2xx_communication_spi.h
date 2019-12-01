/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       AT86RFxx SPI communication interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF2XX_COMMUNICATION_SPI_H
#define AT86RF2XX_COMMUNICATION_SPI_H

#include <stdint.h>
#include <stdio.h>
#include "periph/spi.h"
#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI access specifiers
 * @{
 */
#define AT86RF2XX_ACCESS_REG                                    (0x80)
#define AT86RF2XX_ACCESS_FB                                     (0x20)
#define AT86RF2XX_ACCESS_SRAM                                   (0x00)
#define AT86RF2XX_ACCESS_READ                                   (0x00)
#define AT86RF2XX_ACCESS_WRITE                                  (0x40)
/** @} */

/**
 * @brief SPI bus shortcut
 */
#define SPIDEV          (dev->params.spi)
/**
 * @brief SPI CS pin shortcut
 */
#define CSPIN           (dev->params.cs_pin)

static const void *acquired = NULL;
/**
 * @name Functions to communicate with transceivers via SPI
 * @{
 */
static inline
void at86rf2xx_spi_get_bus(const at86rf2xx_t *dev)
{
    //printf("[at86rf2xx] acquire ...\n");
    assert(!acquired);
    spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk);
    acquired = dev;
}

static inline
void at86rf2xx_spi_release_bus(const at86rf2xx_t *dev)
{
    spi_release(SPIDEV);
    acquired = NULL;
    //printf("[at86rf2xx] release\n");
}

static inline
uint8_t at86rf2xx_spi_reg_read(const at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t reg = (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ | addr);

    at86rf2xx_spi_get_bus(dev);
    uint8_t value = spi_transfer_reg(SPIDEV, CSPIN, reg, 0);
    at86rf2xx_spi_release_bus(dev);
    return value;
}

static inline
void at86rf2xx_spi_reg_write(const at86rf2xx_t *dev, uint8_t addr,
                             uint8_t value)
{
    uint8_t reg = (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE | addr);

    at86rf2xx_spi_get_bus(dev);
    spi_transfer_reg(SPIDEV, CSPIN, reg, value);
    at86rf2xx_spi_release_bus(dev);
}

static inline
void at86rf2xx_spi_sram_read(const at86rf2xx_t *dev, uint8_t offset,
                             void *data, size_t len)
{
    uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ);

    at86rf2xx_spi_get_bus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
    spi_transfer_byte(SPIDEV, CSPIN, true, offset);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, data, len);
    at86rf2xx_spi_release_bus(dev);
}

static inline
void at86rf2xx_spi_sram_write(const at86rf2xx_t *dev, uint8_t offset,
                              const void *data, size_t len)
{
    uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE);

    at86rf2xx_spi_get_bus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
    spi_transfer_byte(SPIDEV, CSPIN, true, offset);
    spi_transfer_bytes(SPIDEV, CSPIN, false, data, NULL, len);
    at86rf2xx_spi_release_bus(dev);
}

static inline
void at86rf2xx_spi_fb_start(const at86rf2xx_t *dev)
{
    uint8_t reg = AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ;

    at86rf2xx_spi_get_bus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
}

static inline
void at86rf2xx_spi_fb_read(const at86rf2xx_t *dev, void *data, size_t len)
{
    spi_transfer_bytes(SPIDEV, CSPIN, true, NULL, data, len);
}

static inline
void at86rf2xx_spi_fb_stop(const at86rf2xx_t *dev)
{
    /* transfer one byte (which we ignore) to release the chip select */
    spi_transfer_byte(SPIDEV, CSPIN, false, 1);
    at86rf2xx_spi_release_bus(dev);
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_COMMUNICATION_SPI_H */
/** @} */
