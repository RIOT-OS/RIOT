/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mhz19
 *
 * @{
 * @file
 * @brief       Default configuration for MH-Z19
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Christian Manal <manal@uni-bremen.de>
 */


#include "board.h"
#include "mhz19.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the MH-Z19
 * @{
 */
#ifdef MODULE_MHZ19_UART
#ifndef MHZ19_PARAM_UART_DEV
#define MHZ19_PARAM_UART_DEV        UART_DEV(1)
#endif

#ifndef MHZ19_PARAMS
#define MHZ19_PARAMS    { .uart = MHZ19_PARAM_UART_DEV }
#endif
#endif /* MODULE_MHZ19_UART */

#ifdef MODULE_MHZ19_PWM
#ifndef MHZ19_PARAM_PIN
#define MHZ19_PARAM_PIN (GPIO_PIN(0, 0))
#endif

#ifndef MHZ19_PARAMS
#define MHZ19_PARAMS    { .pin = MHZ19_PARAM_PIN }
#endif
#endif /* MODULE_MHZ19_PWM */

#ifndef MHZ19_SAUL_INFO
#define MHZ19_SAUL_INFO { .name = "mh-z19" }
#endif
/** @} */

/**
 * @brief   Configure MHZ19
 */
static const mhz19_params_t mhz19_params[] =
{
    MHZ19_PARAMS
};

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This two dimensional array contains static details of the sensors
 * for each device. Please be awar that the indexes are used in
 * auto_init_mhz280, so make sure the indexes match.
 */
static const saul_reg_info_t mhz19_saul_info[] =
{
    MHZ19_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
