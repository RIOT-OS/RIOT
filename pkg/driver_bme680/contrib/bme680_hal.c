/*
 * Copyright (C) 2019 Mesotic SAS
 *               2020 Gunar Schorcht
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_bme680
 * @ingroup     drivers_bme680
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "bme680.h"
#include "bme680_hal.h"
#include "irq.h"
#include "macros/units.h"

#ifdef MODULE_PERIPH_I2C
#include "periph/i2c.h"
#endif
#ifdef MODULE_PERIPH_SPI
#include "periph/spi.h"
#endif

#include "ztimer.h"

#ifndef BME680_SPI_SPEED
#define BME680_SPI_SPEED    MHZ(10)
#endif /* BME680_SPI_SPEED */

#ifndef BME680_SPI_MODE
#define BME680_SPI_MODE     (SPI_MODE_0)
#endif /* BME680_SPI_MODE */

void bme680_ms_sleep(uint32_t msleep)
{
    ztimer_sleep(ZTIMER_MSEC, msleep);
}

#ifdef MODULE_PERIPH_I2C
int8_t bme680_i2c_read_hal(uint8_t dev_id, uint8_t reg_addr,
                           uint8_t *data, uint16_t len)
{
    assert(dev_id < bme680_devs_numof);

    bme680_intf_i2c_t* intf = &(bme680_devs[dev_id]->intf.i2c);
    uint8_t ret;

    i2c_acquire(intf->dev);
    ret = i2c_read_regs(intf->dev, intf->addr, reg_addr, data, len, 0);
    i2c_release(intf->dev);
    return ret;
}

int8_t bme680_i2c_write_hal(uint8_t dev_id, uint8_t reg_addr,
                            uint8_t *data, uint16_t len)
{
    assert(dev_id < bme680_devs_numof);

    bme680_intf_i2c_t* intf = &(bme680_devs[dev_id]->intf.i2c);
    uint8_t ret;

    i2c_acquire(intf->dev);
    ret = i2c_write_regs(intf->dev, intf->addr, reg_addr, data, len, 0);
    i2c_release(intf->dev);
    return ret;
}
#endif /* MODULE_PERIPH_I2C */

#ifdef MODULE_PERIPH_SPI
static inline spi_clk_t spi_clk_cache(spi_t bus, uint32_t freq)
{
    static uint32_t freq_cache;
    static spi_clk_t clk_cache;

    if (freq != freq_cache) {
        freq_cache = freq;
        clk_cache = spi_get_clk(bus, freq);
    }

    return clk_cache;
}

int8_t bme680_spi_read_hal(uint8_t dev_id, uint8_t reg_addr,
                           uint8_t *data, uint16_t len)
{
    assert(dev_id < bme680_devs_numof);

    bme680_intf_spi_t* intf = &(bme680_devs[dev_id]->intf.spi);
    unsigned int cpsr = irq_disable();

    gpio_clear(intf->nss_pin);
    spi_acquire(intf->dev, SPI_CS_UNDEF, BME680_SPI_MODE, spi_clk_cache(intf->dev, BME680_SPI_SPEED));
    spi_transfer_regs(intf->dev, SPI_CS_UNDEF, reg_addr, NULL, data, len);
    gpio_set(intf->nss_pin);

    irq_restore(cpsr);
    spi_release(intf->dev);
    return 0;
}

int8_t bme680_spi_write_hal(uint8_t dev_id, uint8_t reg_addr,
                            uint8_t *data, uint16_t len)
{
    assert(dev_id < bme680_devs_numof);

    bme680_intf_spi_t* intf = &(bme680_devs[dev_id]->intf.spi);
    unsigned int cpsr = irq_disable();

    gpio_clear(intf->nss_pin);
    spi_acquire(intf->dev, SPI_CS_UNDEF, BME680_SPI_MODE, spi_clk_cache(intf->dev, BME680_SPI_SPEED));
    spi_transfer_regs(intf->dev, SPI_CS_UNDEF, reg_addr, data, NULL, len);
    gpio_set(intf->nss_pin);

    irq_restore(cpsr);
    spi_release(intf->dev);
    return 0;
}
#endif /* MODULE_PERIPH_SPI */
