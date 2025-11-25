/*
 * SPDX-FileCopyrightText: 2018 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_ds3234  DS3234 Extremely Accurate SPI RTC
 * @ingroup     drivers_sensors

 * @brief       Driver for Maxim DS3234 Extremely Accurate SPI Bus RTC with
 *              Integrated Crystal and SRAM

 * @{
 *
 * @file
 * @brief       DS3234 device driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <periph/gpio.h>
#include <periph/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parameters for the DS3234 device driver
 */
typedef struct {
    spi_t spi;              /**< SPI bus the sensor is connected to */
    spi_clk_t clk;          /**< SPI bus clock speed */
    gpio_t cs;              /**< CS pin GPIO handle */
} ds3234_params_t;

/**
 * @brief   Return codes for the DS3234 device driver
 */
enum ds3234_return_codes {
    DS3234_OK = 0,
    DS3234_NO_DEV = 1,
    DS3234_NO_SPI = 2
};

/**
 * @brief   Initialize the DS3234 RTC as a PPS device
 *
 * This will enable square wave output on the SQW pin at 1 Hz
 *
 * @param  params     DS3234 circuit parameters
 *
 * @return DS3234_OK        on success
 * @return DS3234_NO_SPI    if cannot initiate SPI
 * @return DS3234_NO_DEV    if the device is not found on the bus
 */
int ds3234_pps_init(const ds3234_params_t *params);

#ifdef __cplusplus
}
#endif

/** @} */
