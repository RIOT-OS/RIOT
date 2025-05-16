/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ina2xx
 *
 * @{
 * @file
 * @brief       Default configuration for INA2xx power/current monitors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */


#include "board.h"
#include "ina2xx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the INA2XX
 * @{
 */
#ifndef INA2XX_PARAM_I2C
/**
 * @brief   Default to first I2C device
 */
#define INA2XX_PARAM_I2C            (I2C_DEV(0))
#endif
#ifndef INA2XX_PARAM_ADDR
/**
 * @brief   Default to address 0x40, which is active if A0 and A1 are connected
 *          to GND
 *
 * On the popular INA219 breakout board this is the default address if solder
 * jumpers remain open.
 */
#define INA2XX_PARAM_ADDR           (0x40)
#endif
#ifndef INA2XX_PARAM_CONFIG
/**
 * @brief   Default to an optimal configuration for current/power measurements
 *          of USB high-power devices using the popular INA219 break out board
 *          with 128 samples averaged
 *
 * | Setting             | Value                                         |
 * |:------------------- |:--------------------------------------------- |
 * | Mode                | Continuous shunt and bus voltage measurements |
 * | Shunt ADC Setting   | 128 Samples, 68.10 ms per conversion          |
 * | Bus ADC Setting     | 128 Samples, 68.10 ms per conversion          |
 * | Shunt Voltage Range | ±80mV                                         |
 * | Bus Voltage Range   | 16V                                           |
 */
#define INA2XX_PARAM_CONFIG         (INA2XX_MODE_CONTINUOUS_SHUNT_BUS | \
                                     INA2XX_SADC_AVG_128_SAMPLES | \
                                     INA2XX_BADC_AVG_128_SAMPLES | \
                                     INA2XX_SHUNT_RANGE_80MV | \
                                     INA2XX_BUS_RANGE_16V)
#endif
#ifndef INA2XX_PARAM_RSHUNT_MOHM
/**
 * @brief   Default to 100 mΩ as shunt resistor
 *
 * This is the value used in the popular INA219 breakout board.
 */
#define INA2XX_PARAM_RSHUNT_MOHM    (100)
#endif
#ifndef INA2XX_PARAM_I_RANGE
/**
 * @brief   Default to a current range of ±655.36mA
 *
 * This is the highest resolution suitable to measure USB high-power devices
 * (up to 500 mA).
 */
#define INA2XX_PARAM_I_RANGE    (INA2XX_CURRENT_RANGE_655_MA)
#endif
#ifndef INA2XX_PARAMS
#define INA2XX_PARAMS               { .i2c         = INA2XX_PARAM_I2C,  \
                                      .addr        = INA2XX_PARAM_ADDR, \
                                      .config      = INA2XX_PARAM_CONFIG, \
                                      .rshunt_mohm = INA2XX_PARAM_RSHUNT_MOHM, \
                                      .i_range     = INA2XX_PARAM_I_RANGE }
#endif
/**@}*/

/**
 * @name    Set default SAUL info for the INA2XX
 * @{
 */
#ifndef INA2XX_SAULINFO
#define INA2XX_SAULINFO             { .name = "INA2XX current" }, \
                                    { .name = "INA2XX power" }, \
                                    { .name = "INA2XX voltage" }
#endif
/**@}*/

/**
 * @brief   Configure INA2XX devices
 */
static const ina2xx_params_t ina2xx_params[] =
{
    INA2XX_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t ina2xx_saul_info[] =
{
    INA2XX_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
