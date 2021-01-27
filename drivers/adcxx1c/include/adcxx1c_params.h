/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adcxx1c
 * @{
 *
 * @file
 * @brief       Default configuration for ADCXX1C devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifndef ADCXX1C_PARAMS_H
#define ADCXX1C_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "adcxx1c.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief  I2C device to use */
#ifndef ADCXX1C_PARAM_I2C
#define ADCXX1C_PARAM_I2C                           (I2C_DEV(0))
#endif

/** @brief  I2C address */
#ifndef ADCXX1C_PARAM_ADDR
#define ADCXX1C_PARAM_ADDR                          (CONFIG_ADCXX1C_I2C_ADDRESS)
#endif

/** @brief  Resolution in bits */
#ifndef ADCXX1C_PARAM_BITS
#define ADCXX1C_PARAM_BITS                          (ADCXX1C_RES_DEFAULT)
#endif

/** @brief  Alert gpio pin */
#ifndef ADCXX1C_PARAM_ALERT_PIN
#define ADCXX1C_PARAM_ALERT_PIN                     (GPIO_UNDEF)
#endif

/**
 * @defgroup drivers_adcxx1c_config   ADCXX1C driver compile configurations
 * @ingroup drivers_adcxx1c
 * @ingroup config_drivers_sensors
 * @{
 */
/** @brief  Conversion interval
 *
 * The automatic conversion mode configures the ADC to continually perform
 * conversions without receiving "read" instructions from the controller over
 * the I2C interface. If enabled, the sample rate for the conversion is set by
 * the cycle time bits which offers up to 7 modes of operation, i.e. multipliers
 * from x32 to x2048 to the base conversion time (Tconvert).
*/
#if IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_DISABLE)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_DISABLED)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_32)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_32)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_64)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_64)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_128)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_128)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_256)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_256)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_512)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_512)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_1024)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_1024)
#elif IS_ACTIVE(CONFIG_ADCXX1C_PARAM_CYCLE_2048)
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_2048)
#endif

#ifndef CONFIG_ADCXX1C_PARAM_CYCLE
#define CONFIG_ADCXX1C_PARAM_CYCLE                  (ADCXX1C_CYCLE_DISABLED)
#endif

/** @brief  Low limit for the alert */
#ifndef CONFIG_ADCXX1C_PARAM_LOW_LIMIT
#define CONFIG_ADCXX1C_PARAM_LOW_LIMIT              (0)
#endif

/** @brief  High limit for the alert */
#ifndef CONFIG_ADCXX1C_PARAM_HIGH_LIMIT
#define CONFIG_ADCXX1C_PARAM_HIGH_LIMIT             (0)
#endif

/** @brief  Hysteresis for the alert
 *
 * Configure the higher limit threshold used to determine the alert condition.
 * After a Vhigh or Vlow alert occurs,the conversion result must move within
 * the Vhigh or Vlow limit by more than this value to clear the alert condition.
*/
#ifndef CONFIG_ADCXX1C_PARAM_HYSTERESIS
#define CONFIG_ADCXX1C_PARAM_HYSTERESIS             (0)
#endif
/** @} */

/**
 * @brief   ADCXX1C driver configuration structures
 */
#ifndef ADCXX1C_PARAMS
#define ADCXX1C_PARAMS     { .i2c        = ADCXX1C_PARAM_I2C,               \
                             .addr       = ADCXX1C_PARAM_ADDR,              \
                             .bits       = ADCXX1C_PARAM_BITS,              \
                             .cycle      = CONFIG_ADCXX1C_PARAM_CYCLE,      \
                             .alert_pin  = ADCXX1C_PARAM_ALERT_PIN,         \
                             .low_limit  = CONFIG_ADCXX1C_PARAM_LOW_LIMIT,  \
                             .high_limit = CONFIG_ADCXX1C_PARAM_HIGH_LIMIT, \
                             .hysteresis = CONFIG_ADCXX1C_PARAM_HYSTERESIS }
#endif

/**
 * @brief   ADCXX1C driver SAUL registry information structures
 */
#ifndef ADCXX1C_SAUL_INFO
#define ADCXX1C_SAUL_INFO       { .name = "adcxx1c" }
#endif

/**
 * @brief   ADCXX1C configuration
 */
static const adcxx1c_params_t adcxx1c_params[] =
{
    ADCXX1C_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t adcxx1c_saul_info[] =
{
    ADCXX1C_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* ADCXX1C_PARAMS_H */
