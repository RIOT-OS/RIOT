/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_tps6274x TPS6274x
 * @ingroup     drivers_power
 * @brief       Device driver interface for the TPS6274x DC-DC Converter
 * @{
 *
 * @file
 * @brief       Device driver interface for the TPS6274x DC-DC Converter
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>
#include "periph/gpio.h"

/**
 * @brief   TPS6274x Configuration struct
 */
typedef struct {
    gpio_t vsel[4];            /**< select line pin mapping */
    gpio_t ctrl_pin;           /**< ctrl pin  mapping */
} tps6274x_params_t;

/**
 * @brief   Device descriptor for the TPS6274x
 */
typedef struct {
    tps6274x_params_t  params; /**< device initialization parameters */
} tps6274x_t;

/**
 * @brief   Status and error return codes
 */
enum {
    TPS6274X_OK = 0,           /**< everything was fine */
    TPS6274X_ERR_INIT,         /**< error during init */
};

/**
 * @brief   Init converter
 *
 * @param[in] dev      Initialized device descriptor for TPS6274x device
 * @param[in] params   Initialization parameters
 *
 * @return              set voltage in mV
 */
int tps6274x_init(tps6274x_t *dev, const tps6274x_params_t *params);

/**
 * @brief   Switch to different voltage level
 *
 * @param[in] dev       Device descriptor for TPS6274x device
 * @param[in] voltage   Voltage to set in mV (needs to be between 1.8V-3.3V
 * @return              the voltage that was set in mV
 */
uint16_t tps6274x_switch_voltage(tps6274x_t *dev, uint16_t voltage);

/**
 * @brief   Sets ctrl pin high to power a subsystem connected on the load pin
 *
 * @param[in] dev       Device descriptor for TPS6274x device
 * @param[in] status    0 will disable the load, everything else will activate it
 */
void tps6274x_load_ctrl(tps6274x_t *dev, int status);

#ifdef __cplusplus
}
#endif
/** @} */
