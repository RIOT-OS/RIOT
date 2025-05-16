/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ina3221
 * @{
 *
 * @file
 * @brief       Driver configuration parameters for Texas Instruments INA3221
 *              three-channel, high-side current and bus voltage
 *              monitor
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */


#include "board.h"
#include "ina3221.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INA3221_PARAM_I2C
#define INA3221_PARAM_I2C               (I2C_DEV(0))        /**< I2C bus index */
#endif

#ifndef INA3221_PARAM_ADDR
#define INA3221_PARAM_ADDR              (INA3221_ADDR_00)   /**< I2C device address */
#endif

#ifndef INA3221_PARAM_PIN_WRN
#define INA3221_PARAM_PIN_WRN           (GPIO_UNDEF)        /**< Warning alert pin */
#endif

#ifndef INA3221_PARAM_PIN_CRT
#define INA3221_PARAM_PIN_CRT           (GPIO_UNDEF)        /**< Critical alert pin */
#endif

#ifndef INA3221_PARAM_PIN_PV
#define INA3221_PARAM_PIN_PV            (GPIO_UNDEF)        /**< Power valid alert pin */
#endif

#ifndef INA3221_PARAM_PIN_TC
#define INA3221_PARAM_PIN_TC            (GPIO_UNDEF)        /**< Timing control alert pin */
#endif

#ifndef INA3221_PARAM_INT_PU_PIN_WRN
#define INA3221_PARAM_INT_PU_PIN_WRN    (0)                 /**< Enable or disable internal pull up resistor for pin WRN */
#endif

#ifndef INA3221_PARAM_INT_PU_PIN_CRT
#define INA3221_PARAM_INT_PU_PIN_CRT    (0)                 /**< Enable or disable internal pull up resistor for pin CRT */
#endif

#ifndef INA3221_PARAM_INT_PU_PIN_PV
#define INA3221_PARAM_INT_PU_PIN_PV     (0)                 /**< Enable or disable internal pull up resistor for pin PV */
#endif

#ifndef INA3221_PARAM_INT_PU_PIN_TC
#define INA3221_PARAM_INT_PU_PIN_TC     (0)                 /**< Enable or disable internal pull up resistor for pin TC */
#endif

#ifndef INA3221_PARAM_CONFIG
#define INA3221_PARAM_CONFIG    (          \
        INA3221_ENABLE_CH1 |               \
        INA3221_ENABLE_CH2 |               \
        INA3221_ENABLE_CH3 |               \
        INA3221_NUM_SAMPLES_4 |            \
        INA3221_CONV_TIME_BADC_4156US |    \
        INA3221_CONV_TIME_SADC_4156US |    \
        INA3221_MODE_CONTINUOUS_SHUNT_BUS  \
        )                                                   /**< Configuration register value */
#endif

#ifndef INA3221_PARAM_RSHUNT_MOHM_CH1
#define INA3221_PARAM_RSHUNT_MOHM_CH1   (100)               /**< Channel 1 shunt resistance */
#endif

#ifndef INA3221_PARAM_RSHUNT_MOHM_CH2
#define INA3221_PARAM_RSHUNT_MOHM_CH2   (100)               /**< Channel 2 shunt resistance */
#endif

#ifndef INA3221_PARAM_RSHUNT_MOHM_CH3
#define INA3221_PARAM_RSHUNT_MOHM_CH3   (100)               /**< Channel 3 shunt resistance */
#endif

#ifndef INA3221_PARAMS
#define INA3221_PARAMS  {                                                      \
        .i2c = INA3221_PARAM_I2C,                                              \
        .addr = INA3221_PARAM_ADDR,                                            \
        .upins.pins = {                                                        \
            .pin_warn = INA3221_PARAM_PIN_WRN,                                 \
            .pin_crit = INA3221_PARAM_PIN_CRT,                                 \
            .pin_tc = INA3221_PARAM_PIN_TC,                                    \
            .pin_pv = INA3221_PARAM_PIN_PV                                     \
        },                                                                     \
        .gpio_config = (INA3221_PARAM_INT_PU_PIN_WRN << INA3221_ALERT_WRN) |   \
                       (INA3221_PARAM_INT_PU_PIN_CRT << INA3221_ALERT_CRT) |   \
                       (INA3221_PARAM_INT_PU_PIN_TC << INA3221_ALERT_TC)   |   \
                       (INA3221_PARAM_INT_PU_PIN_PV << INA3221_ALERT_PV),      \
        .config = INA3221_PARAM_CONFIG,                                        \
        .rshunt_mohm = {                                                       \
            INA3221_PARAM_RSHUNT_MOHM_CH1,                                     \
            INA3221_PARAM_RSHUNT_MOHM_CH2,                                     \
            INA3221_PARAM_RSHUNT_MOHM_CH3                                      \
        }                                                                      \
}                                                           /**< Default device initialization parameters */
#endif

#ifndef INA3221_SAUL_INFO
#define INA3221_SAUL_INFO   { .name = "INA3221 bus voltage" }, \
    { .name = "INA3221 current" },                             \
    { .name = "INA3221 power" },                               \
    { .name = "INA3221 shunt voltage sum" }                              /**< SAUL driver information */
#endif

/**
 * @brief INA3221 array of device configurations
 */
static const ina3221_params_t ina3221_params[] = {
    INA3221_PARAMS
};

/**
 * @brief INA3221 array of SAUL driver information
 */
static const saul_reg_info_t ina3221_saul_info[] = {
    INA3221_SAUL_INFO
};
#ifdef __cplusplus
}
#endif

/** @} */
