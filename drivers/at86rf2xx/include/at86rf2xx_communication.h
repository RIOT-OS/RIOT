/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
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
 * @brief       SPI based and peripheral communication interface
 *              for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_COMMUNICATION_H
#define AT86RF2XX_COMMUNICATION_H

#include <stdint.h>

#include "at86rf2xx.h"

#if IS_USED(MODULE_AT86RF2XX_SPI)
#include "at86rf2xx_communication_spi.h"
#endif
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
#include "at86rf2xx_communication_periph.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to read from
 * @param[in] addr      address of the register to read
 *
 * @return              the value of the specified register
 */
uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, uint8_t addr);

/**
 * @brief   Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to write to
 * @param[in] addr      address of the register to write
 * @param[in] value     value to write to the given register
 */
void at86rf2xx_reg_write(const at86rf2xx_t *dev, uint8_t addr, uint8_t value);

/**
 * @brief   Read a chunk of data from the SRAM of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from [valid 0x00-0x7f]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from SRAM
 */
void at86rf2xx_sram_read(const at86rf2xx_t *dev, uint8_t offset,
                         void *data, size_t len);
/**
 * @brief   Write a chunk of data into the SRAM of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the SRAM to write to [valid 0x00-0x7f]
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void at86rf2xx_sram_write(const at86rf2xx_t *dev, uint8_t offset,
                          const void *data, size_t len);

/**
 * @brief   Start a read transcation internal frame buffer of the given device
 *
 * Reading the frame buffer returns some extra bytes that are not accessible
 * through reading the RAM directly. This locks the used SPI.
 *
 * @param[in]  dev      device to start read
 */
void at86rf2xx_fb_start(const at86rf2xx_t *dev);

/**
 * @brief   Read the internal frame buffer of the given device
 *
 * Each read advances the position in the buffer by @p len.
 *
 * @param[in]  dev      device to read from
 * @param[out] data     buffer to copy the data to
 * @param[in]  len      number of bytes to read from the frame buffer
 */
void at86rf2xx_fb_read(const at86rf2xx_t *dev, void *data, size_t len);

/**
 * @brief   Stop a read transcation internal frame buffer of the given device
 *
 * Release the SPI device and unlock frame buffer protection.
 *
 * @param[in]  dev      device to stop read
 */
void at86rf2xx_fb_stop(const at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_COMMUNICATION_H */
/** @} */
