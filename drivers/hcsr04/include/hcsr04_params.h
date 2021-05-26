/*
 * Copyright (C) 2021 AUTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hcsr04
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Evripidis Chondromatidis <eurichon1996@gmail.com>
 */

#ifndef HCSR04_PARAMS_H
#define HCSR04_PARAMS_H

#include "board.h"
#include "hcsr04.h"
#include "hcsr04_constants.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for HC-SR04 sensor
 * @{
 */
#ifndef AIR_TEMPERATURE                           
#define AIR_TEMPERATURE                     (24000U)    
#endif
#ifndef HCSR04_TRIGGER_PIN                           
#define HCSR04_TRIGGER_PIN                  (GPIO_PIN(0,2))    
#endif
#ifndef HCSR04_ECHO_PIN                           
#define HCSR04_ECHO_PIN                     (GPIO_PIN(0,4))    
#endif

#ifndef HCSR04_PARAMS
/**
 * @brief   HC-SR04 default initialization parameters
 */
#define HCSR04_PARAMS                       { \
                                                .temperature = AIR_TEMPERATURE,  \
                                                .trigger_pin = HCSR04_TRIGGER_PIN,  \
                                                .echo_pin = HCSR04_ECHO_PIN,  \
                                            }                                                
#endif
#ifndef HCSR04_SAUL_INFO
#define HCSR04_SAUL_INFO                    { .name = "hcsr04" }
#endif
/**@}*/

/**
 * @brief   Initialization parameters for HC-SR04 devices
 */
static const hcsr04_params_t hcsr04_params[] =
{
    HCSR04_PARAMS
};

static const saul_reg_info_t hcsr04_saul_info[] =
{
    HCSR04_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* HCSR04_PARAMS_H */
/** @} */
