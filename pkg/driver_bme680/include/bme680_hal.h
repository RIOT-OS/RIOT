/*
 * SPDX-FileCopyrightText: 2018 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_driver_bme680
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

void bme680_ms_sleep(uint32_t msleep);

#ifdef MODULE_PERIPH_I2C

int8_t bme680_i2c_read_hal(uint8_t dev_id, uint8_t reg_addr,
                           uint8_t *data, uint16_t len);
int8_t bme680_i2c_write_hal(uint8_t dev_id, uint8_t reg_addr,
                            uint8_t *data, uint16_t len);
#endif

#ifdef MODULE_PERIPH_SPI

int8_t bme680_spi_read_hal(uint8_t dev_id, uint8_t reg_addr,
                           uint8_t *data, uint16_t len);
int8_t bme680_spi_write_hal(uint8_t dev_id, uint8_t reg_addr,
                            uint8_t *data, uint16_t len);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
