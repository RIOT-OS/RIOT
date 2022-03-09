/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_apds99xx
 * @brief       Default configuration for Broadcom APDS99XX proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef APDS99XX_PARAMS_H
#define APDS99XX_PARAMS_H

#include "board.h"
#include "apds99xx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef APDS99XX_PARAM_DEV
/** device is I2C_DEV(0) */
#define APDS99XX_PARAM_DEV          I2C_DEV(0)
#endif

#ifndef APDS99XX_PARAM_ALS_STEPS
/** ALS measurement: enabled with integration time of 64 steps */
#define APDS99XX_PARAM_ALS_STEPS    (64)
#endif
#ifndef APDS99XX_PARAM_ALS_GAIN
/** ALS gain: 1 x gain */
#define APDS99XX_PARAM_ALS_GAIN     (APDS99XX_ALS_GAIN_1)
#endif

#ifndef APDS99XX_PARAM_PRX_PULSES
/** PRX LED pulse count: 8 pulses as recommended in datasheet */
#define APDS99XX_PARAM_PRX_PULSES   (8)
#endif
#ifndef APDS99XX_PARAM_PRX_DRIVE
/** PRX LED drive strength: 100 mA as recommended in datasheet */
#define APDS99XX_PARAM_PRX_DRIVE    (APDS99XX_PRX_DRIVE_100)
#endif
#ifndef APDS99XX_PARAM_PRX_GAIN
/** PRX gain: 1 x gain */
#define APDS99XX_PARAM_PRX_GAIN     (APDS99XX_PRX_GAIN_1)
#endif
#ifndef APDS99XX_PARAM_WAIT_STEPS
/** Waiting time: disabled */
#define APDS99XX_PARAM_WAIT_STEPS    (0)
#endif
#ifndef APDS99XX_PARAM_WAIT_LONG
/** Wait long: false */
#define APDS99XX_PARAM_WAIT_LONG    (false)
#endif

#ifndef APDS99XX_PARAM_INT_PIN
/** Interrupt pin */
#define APDS99XX_PARAM_INT_PIN      (GPIO_PIN(0, 0))
#endif

#ifndef APDS99XX_PARAMS
#define APDS99XX_PARAMS { \
                            .dev = APDS99XX_PARAM_DEV,  \
                            .als_steps = APDS99XX_PARAM_ALS_STEPS, \
                            .als_gain = APDS99XX_PARAM_ALS_GAIN, \
                            .prx_pulses = APDS99XX_PARAM_PRX_PULSES, \
                            .prx_gain = APDS99XX_PARAM_PRX_GAIN, \
                            .prx_drive = APDS99XX_PARAM_PRX_DRIVE, \
                            .wait_steps = APDS99XX_PARAM_WAIT_STEPS, \
                            .wait_long = APDS99XX_PARAM_WAIT_LONG, \
                            .int_pin = APDS99XX_PARAM_INT_PIN, \
                        }
#endif

#ifndef APDS99XX_SAUL_INFO
#define APDS99XX_SAUL_INFO          { .name = "apds99xx" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const apds99xx_params_t apds99xx_params[] =
{
    APDS99XX_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t apds99xx_saul_info[] =
{
    APDS99XX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* APDS99XX_PARAMS_H */
/** @} */
