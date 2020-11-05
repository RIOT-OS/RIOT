/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gp2y10xx
 * @{
 *
 * @file
 * @brief       Default configuration for GP2Y10xx devices
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#ifndef GP2Y10XX_PARAMS_H
#define GP2Y10XX_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "gp2y10xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_gp2y10xx_config   GP2Y10xx driver compile configurations
 * @ingroup drivers_gp2y10xx
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   ADC line to use
 */
#ifndef GP2Y10XX_PARAM_AOUT
#define GP2Y10XX_PARAM_AOUT     (ADC_LINE(0))
#endif

/**
 * @brief   ADC line resolution
 */
#ifndef GP2Y10XX_PARAM_ADC_RES
#define GP2Y10XX_PARAM_ADC_RES  (ADC_RES_10BIT)
#endif

/**
 * @brief   Reference voltage used for the VCC supply of the sensor, in mV.
 */
#ifndef GP2Y10XX_PARAM_VREF
#define GP2Y10XX_PARAM_VREF     (3300)
#endif

/**
 * @brief   ILED GPIO pin
 */
#ifndef GP2Y10XX_PARAM_ILED_PIN
#define GP2Y10XX_PARAM_ILED_PIN (GPIO_UNDEF)
#endif

/**
 * @brief   ILED level, can be active-high or active-low.
 */
#ifndef GP2Y10XX_PARAM_ILED_LEVEL
#define GP2Y10XX_PARAM_ILED_LEVEL (GP2Y10XX_ILED_LEVEL_HIGH)
#endif

/**
 * @brief   GP2Y10xx driver configuration parameters
 */
#ifndef GP2Y10XX_PARAMS
#define GP2Y10XX_PARAMS     { .aout         = GP2Y10XX_PARAM_AOUT,      \
                              .adc_res      = GP2Y10XX_PARAM_ADC_RES,   \
                              .vref         = GP2Y10XX_PARAM_VREF,      \
                              .iled_pin     = GP2Y10XX_PARAM_ILED_PIN,  \
                              .iled_level   = GP2Y10XX_PARAM_ILED_LEVEL, \
                            }
#endif
/** @} */

/**
 * @brief   GP2Y10xx driver SAUL registry information structures
 */
#ifndef GP2Y10XX_SAUL_INFO
#define GP2Y10XX_SAUL_INFO  { .name = "gp2y1010" }
#endif

/**
 * @brief   GP2Y1010 configuration
 */
static const gp2y10xx_params_t gp2y10xx_params[] =
{
    GP2Y10XX_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t gp2y10xx_saul_info[] =
{
    GP2Y10XX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* GP2Y10XX_PARAMS_H */
