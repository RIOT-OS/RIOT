/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mh-z19
 *
 * @{
 * @file
 * @brief       Default configuration for MH-Z19
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MHZ19_PARAMS_H
#define MHZ19_PARAMS_H

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
#ifndef MHZ19_PARAM_UART_DEV
#define MHZ19_PARAM_UART_DEV        UART_DEV(1)
#endif

#ifndef MHZ19_PARAMS
#define MHZ19_PARAMS    { .uart = MHZ19_PARAM_UART_DEV }
#endif

#ifndef MHZ19_SAUL_INFO
#define MHZ19_SAUL_INFO { .name = "mh-z19" }
#endif
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

#endif /* MHZ19_PARAMS_H */
/** @} */
