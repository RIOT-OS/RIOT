/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
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
 * @brief       Implementation of SPI based and peripheral communication
 *              interface for AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include "kernel_defines.h"
#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t value = 0;

    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            value = at86rf2xx_spi_reg_read(dev, addr);
#else
            (void)dev;
            (void)addr;
#endif
            break;
        }
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            value = at86rf2xx_periph_reg_read(AT86RFA1_REG(addr));
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            value = at86rf2xx_periph_reg_read(AT86RFR2_REG(addr));
        }
#endif
    }
    return value;
}

void at86rf2xx_reg_write(const at86rf2xx_t *dev, uint8_t addr, uint8_t value)
{
    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_reg_write(dev, addr, value);
#else
            (void)dev;
            (void)addr;
            (void)value;
#endif
            break;
        }
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_periph_reg_write(AT86RFA1_REG(addr), value);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_periph_reg_write(AT86RFR2_REG(addr), value);
            break;
        }
#endif
    }
}

void at86rf2xx_sram_read(const at86rf2xx_t *dev, uint8_t offset,
                         void *data, size_t len)
{
    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_sram_read(dev, offset, data, len);
#else
            (void)dev;
            (void)offset;
            (void)data;
            (void)len;
#endif
        }
        break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_periph_sram_read(AT86RFA1_REG__TRXFBST,
                                       offset, data, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_periph_sram_read(AT86RFR2_REG__TRXFBST,
                                       offset, data, len);
            break;
        }
#endif
    }
}

void at86rf2xx_sram_write(const at86rf2xx_t *dev, uint8_t offset,
                          const void *data, size_t len)
{
    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_sram_write(dev, offset, data, len);
#else
            (void)dev;
            (void)offset;
            (void)data;
            (void)len;
#endif
            break;
        }
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_periph_sram_write(AT86RFA1_REG__TRXFBST,
                                        offset, data, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_periph_sram_write(AT86RFR2_REG__TRXFBST,
                                        offset, data, len);
            break;
        }
#endif
    }
}

void at86rf2xx_fb_start(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_fb_start(dev);
#else
            (void)dev;
#endif
            break;
        }
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            break;
#endif
    }
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       void *data, size_t len)
{
    switch (dev->base.dev_type) {
        default:
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_fb_read(dev, data, len);
#else
            (void)dev;
            (void)data;
            (void)len;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_periph_fb_read(data, AT86RFA1_REG__TRXFBST, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_periph_fb_read(data, AT86RFR2_REG__TRXFBST, len);
            break;
        }
#endif
    }
}

void at86rf2xx_fb_stop(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:
#if IS_USED(MODULE_AT86RF2XX_SPI)
            at86rf2xx_spi_fb_stop(dev);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            break;
#endif
    }
}
