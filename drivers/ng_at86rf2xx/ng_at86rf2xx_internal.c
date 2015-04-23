/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
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
#include "ng_at86rf2xx_internal.h"
#include "ng_at86rf2xx_registers.h"

void ng_at86rf2xx_reg_write(const ng_at86rf2xx_t *dev,
                            const uint8_t addr,
                            const uint8_t value)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     NG_AT86RF2XX_ACCESS_REG | NG_AT86RF2XX_ACCESS_WRITE | addr,
                     value, 0);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

uint8_t ng_at86rf2xx_reg_read(const ng_at86rf2xx_t *dev, const uint8_t addr)
{
    char value;

    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     NG_AT86RF2XX_ACCESS_REG | NG_AT86RF2XX_ACCESS_READ | addr,
                     0, &value);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);

    return (uint8_t)value;
}

void ng_at86rf2xx_sram_read(const ng_at86rf2xx_t *dev,
                            const uint8_t offset,
                            uint8_t *data,
                            const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     NG_AT86RF2XX_ACCESS_SRAM | NG_AT86RF2XX_ACCESS_READ,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->spi, NULL, (char*)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void ng_at86rf2xx_sram_write(const ng_at86rf2xx_t *dev,
                             const uint8_t offset,
                             const uint8_t *data,
                             const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_reg(dev->spi,
                     NG_AT86RF2XX_ACCESS_SRAM | NG_AT86RF2XX_ACCESS_WRITE,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->spi, (char*)data, NULL, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void ng_at86rf2xx_fb_read(const ng_at86rf2xx_t *dev,
                          uint8_t *data,
                          const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_byte(dev->spi,
                      NG_AT86RF2XX_ACCESS_FB | NG_AT86RF2XX_ACCESS_READ,
                      NULL);
    spi_transfer_bytes(dev->spi, NULL, (char *)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

uint8_t ng_at86rf2xx_get_status(const ng_at86rf2xx_t *dev)
{
    return (ng_at86rf2xx_reg_read(dev, NG_AT86RF2XX_REG__TRX_STATUS)
            & NG_AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}
