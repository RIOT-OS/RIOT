/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pir
 *
 * @{
 * @file
 * @brief       Default configuration for PIR devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef PIR_PARAMS_H
#define PIR_PARAMS_H

#include "board.h"
#include "pir.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the PIR driver
 * @{
 */
#ifndef PIR_PARAM_GPIO
#define PIR_PARAM_GPIO             GPIO_PIN(0, 6)
#endif
#ifndef PIR_PARAM_ACTIVE_HIGH
#define PIR_PARAM_ACTIVE_HIGH      1
#endif

#ifndef PIR_PARAMS
#define PIR_PARAMS                 { .gpio = PIR_PARAM_GPIO, \
                                     .active_high = PIR_PARAM_ACTIVE_HIGH }
#endif
#ifndef PIR_SAUL_INFO
#define PIR_SAUL_INFO              { .name = "pir" }
#endif
/**@}*/

/**
 * @brief   PIR configuration
 */
static const pir_params_t pir_params[] =
{
    PIR_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t pir_saul_info[] =
{
    PIR_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* PIR_PARAMS_H */
/** @} */
