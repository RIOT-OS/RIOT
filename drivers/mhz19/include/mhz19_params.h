/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mhz19
 *
 * @{
 * @file
 * @brief       Default configuration for MH-Z19 CO2 sensor
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 */

#ifndef MHZ19_PARAMS_H
#define MHZ19_PARAMS_H

#include "mhz19.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Set default configuration parameters for the mhz19
 * @{
 */
#ifndef MHZ19_PARAM_PIN
#define MHZ19_PARAM_PIN (GPIO_PIN(0, 0))
#endif

#define MHZ19_PARAMS_DEFAULT { .pin = MHZ19_PARAM_PIN }

#ifndef MHZ19_SAUL_INFO
#define MHZ19_SAUL_INFO { .name = "mh-z19" }
#endif

/** @} */


/**
 * @brief  Configure mhz19
 */
static const mhz19_params_t mhz19_params[] =
{
#ifdef MHZ19_PARAMS_BOARD
    MHZ19_PARAMS_BOARD
#else
    MHZ19_PARAMS_DEFAULT
#endif
};

/**
 * @brief  Configure mhz19 SAUL registry entries
 */
static const saul_reg_info_t mhz19_saul_reg_info[] =
{
    MHZ19_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MHZ19_PARAMS_H */
/** @} */
