/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds1307  DS1307 RTC
 * @ingroup     drivers_sensors
 * @brief       Device drive interface for the DS1307 real-time clock
 * @{
 *
 * @file
 * @brief       DS1307 definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef DS1307_H
#define DS1307_H

#include <stdbool.h>
#include <time.h>

#include "nvram.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   I2C address of DS1307 RTC
 */
#define DS1307_I2C_ADDRESS      (0x68)

/**
 * @defgroup drivers_ds1307_config   DS1307 RTC driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */

/**
 * @brief   Maximum I2C bus speed to use with the device
 */
#ifndef DS1307_I2C_MAX_CLK
#define DS1307_I2C_MAX_CLK      (I2C_SPEED_FAST)
#endif
/** @} */

/**
 * @brief   Maximum size in byte of on-chip NVRAM
 */
#define DS1307_NVRAM_MAX_SIZE   (56U)

/**
 * @brief   Device descriptor for DS1307 devices
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    nvram_t nvram;              /**< on-chip NVRAM (see nvram.h) */
} ds1307_t;

/**
 * @brief   Set of configuration parameters for DS1307 devices
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
} ds1307_params_t;

/**
 * @brief   Modes for the DS1307 square wave / output driver
 */
typedef enum {
    DS1307_SQW_MODE_0 = 0x00,       /**< OUT: 0 */
    DS1307_SQW_MODE_1000HZ = 0x10,  /**< SQW: 1kHz */
    DS1307_SQW_MODE_4096HZ = 0x11,  /**< SQW: 4.096 kHz */
    DS1307_SQW_MODE_8192HZ = 0x12,  /**< SQW: 8.192 kHz */
    DS1307_SQW_MODE_32768HZ = 0x13, /**< SQW: 32.768 kHz */
    DS1307_SQW_MODE_1 = 0x80,       /**< OUT: 1 */
} ds1307_sqw_mode_t;

/**
 * @brief   Initialize the given DS1307 device
 *
 * @param[out] dev      device descriptor of the targeted device
 * @param[in] params    device configuration (i2c bus, address and bus clock)
 *
 * @return      0 on success
 * @return      < 0 if unable to speak to the device
 */
int ds1307_init(ds1307_t *dev, const ds1307_params_t *params);

/**
 * @brief   Set RTC to a given time.
 *
 * @param[in] dev       device descriptor of the targeted device
 * @param[in] time      pointer to the struct holding the time to set.
 *
 * @return      0 on success
 * @return      < 0 if unable to speak to the device
 */
int ds1307_set_time(const ds1307_t *dev, const struct tm *time);

/**
 * @brief   Get current RTC time.
 *
 * @param[in] dev       device descriptor of the targeted device
 * @param[out] time     pointer to the struct to write the time to.
 *
 * @return      0 on success
 * @return      < 0 if unable to speak to the device
 */
int ds1307_get_time(const ds1307_t *dev, struct tm *time);

/**
 * @brief   Halt clock
 *
 * @note    Can be reversed using @ref ds1307_set_time()
 *
 * @param[in] dev       device descriptor of the targeted device
 *
 * @return      0 on success
 * @return      < 0 if unable to speak to the device
 */
int ds1307_halt(const ds1307_t *dev);

/**
 * @brief   Set mode of square wave / output driver
 *
 * @note    To get the actual output of the driver, attach the pin via GPIO
 *
 * @param[in] dev       device descriptor of the targeted device
 * @param[in] mode      mode for the square wave / output driver
 *
 * @return      0 on success
 * @return      < 0 if unable to speak to the device
 */
int ds1307_set_sqw_mode(const ds1307_t *dev, ds1307_sqw_mode_t mode);

/**
 * @brief   Get current mode of square wave / output driver
 *
 * @note    To get the actual output of the driver, attach the pin via GPIO
 *
 * @param[in] dev       device descriptor of the targeted device
 *
 * @return  current mode of the square wave / output driver
 *          (see ds1307_sqw_mode_t)
 * @return  < 0 if unable to speak to the device
 */
int ds1307_get_sqw_mode(const ds1307_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DS1307_H */
