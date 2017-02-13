/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Internal interfaces for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef AT86RF2XX_INTERNAL_H
#define AT86RF2XX_INTERNAL_H

#include <stdint.h>

#include "at86rf2xx.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Transition time from SLEEP to TRX_OFF in us, refer figure 7-4, p.42.
 *          For different environments refer figure 13-13, p.201
 */
#define AT86RF2XX_WAKEUP_DELAY          (300U)

/**
 * @brief   Minimum reset pulse width, refer p.190
 */
#define AT86RF2XX_RESET_PULSE_WIDTH     (1U)

/**
 * @brief   Transition time to TRX_OFF after reset pulse in us, refer
 *          figure 7-8, p. 44.
 */
#define AT86RF2XX_RESET_DELAY           (26U)

/**
 * @brief   Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to read from
 * @param[in] addr      address of the register to read
 *
 * @return              the value of the specified register
 */
uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, const uint8_t addr);

/**
 * @brief   Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to write to
 * @param[in] addr      address of the register to write
 * @param[in] value     value to write to the given register
 */
void at86rf2xx_reg_write(const at86rf2xx_t *dev, const uint8_t addr,
                         const uint8_t value);

/**
 * @brief   Read a chunk of data from the SRAM of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from [valid 0x00-0x7f]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from SRAM
 */
void at86rf2xx_sram_read(const at86rf2xx_t *dev,
                         const uint8_t offset,
                         uint8_t *data,
                         const size_t len);

/**
 * @brief   Write a chunk of data into the SRAM of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the SRAM to write to [valid 0x00-0x7f]
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void at86rf2xx_sram_write(const at86rf2xx_t *dev,
                          const uint8_t offset,
                          const uint8_t *data,
                          const size_t len);

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
void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data, const size_t len);

/**
 * @brief   Stop a read transcation internal frame buffer of the given device
 *
 * Release the SPI device and unlock frame buffer protection.
 *
 * @param[in]  dev      device to stop read
 */
void at86rf2xx_fb_stop(const at86rf2xx_t *dev);

/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev);

/**
 * @brief   Make sure that device is not sleeping
 *
 * @param[in] dev       device to eventually wake up
 */
void at86rf2xx_assert_awake(at86rf2xx_t *dev);

/**
 * @brief   Trigger a hardware reset
 *
 * @param[in] dev       device to reset
 */
void at86rf2xx_hardware_reset(at86rf2xx_t *dev);


/**
 * @brief   Set PHY parameters based on channel and page number
 *
 * @param[in] dev       device to configure
 */
void at86rf2xx_configure_phy(at86rf2xx_t *dev);

#if defined(MODULE_AT86RF233) || defined(MODULE_AT86RF231) || defined(DOXYGEN)
/**
 * @brief   Read random data from the RNG
 *
 * @note    According to the data sheet this function only works properly in
 *          Basic Operation Mode. However, sporadic testing has shown that even
 *          in Extended Operation Mode this returns random enough data to be
 *          used as a seed for @ref sys_random if no cryptographically secure
 *          randomness is required.
 *          Any further use-case needs to be evaluated, especially if
 *          crypto-relevant randomness is required.
 *
 * @param[in] dev       device to configure
 * @param[out] data     buffer to copy the random data to
 * @param[in]  len      number of random bytes to store in data
 */
void at86rf2xx_get_random(at86rf2xx_t *dev, uint8_t *data, const size_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_INTERNAL_H */
/** @} */
