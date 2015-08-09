/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/spi.h"
#include "periph/gpio.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"

void at86rf2xx_reg_write(const at86rf2xx_t *dev,
                         const uint8_t addr,
                         const uint8_t value)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE | addr,
                     value, 0);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, const uint8_t addr)
{
    char value;

    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ | addr,
                     0, &value);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);

    return (uint8_t)value;
}

void at86rf2xx_sram_read(const at86rf2xx_t *dev,
                         const uint8_t offset,
                         uint8_t *data,
                         const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->spi, NULL, (char *)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void at86rf2xx_sram_write(const at86rf2xx_t *dev,
                          const uint8_t offset,
                          const uint8_t *data,
                          const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->spi, (char *)data, NULL, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data,
                       const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_byte(dev->spi,
                      AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ,
                      NULL);
    spi_transfer_bytes(dev->spi, NULL, (char *)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev)
{
    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
            & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}
