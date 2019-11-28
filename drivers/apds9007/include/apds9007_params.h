/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_apds9007
 *
 * @{
 * @file
 * @brief       Default configuration for APDS9007 devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef APDS9007_PARAMS_H
#define APDS9007_PARAMS_H

#include "board.h"
#include "apds9007.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the APDS9007 driver
 * @{
 */
#ifndef APDS9007_PARAM_GPIO
#define APDS9007_PARAM_GPIO     GPIO_PIN(PA,6)
#endif
#ifndef APDS9007_PARAM_ADCLINE
#define APDS9007_PARAM_ADCLINE  0
#endif
#ifndef APDS9007_PARAM_ADCRES
#define APDS9007_PARAM_ADCRES   ADC_RES_12BIT
#endif

#ifndef APDS9007_PARAMS
#define APDS9007_PARAMS         { .gpio = APDS9007_PARAM_GPIO, \
                                  .adcline = APDS9007_PARAM_ADCLINE, \
                                  .adcres = APDS9007_PARAM_ADCRES }
#endif
#ifndef APDS9007_SAUL_INFO
#define APDS9007_SAUL_INFO      { .name = "apds9007" }
#endif
/**@}*/

/**
 * @brief   APDS9007 configuration
 */
static const apds9007_params_t apds9007_params[] =
{
    APDS9007_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t apds9007_saul_info[] =
{
    APDS9007_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* APDS9007_PARAMS_H */
/** @} */
