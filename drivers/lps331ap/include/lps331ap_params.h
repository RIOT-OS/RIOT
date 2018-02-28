/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lps331ap
 *
 * @{
 * @file
 * @brief       Default configuration for LPS331AP devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LPS331AP_PARAMS_H
#define LPS331AP_PARAMS_H

#include "board.h"
#include "lps331ap.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LPS331AP_PARAM_I2C
#define LPS331AP_PARAM_I2C              I2C_DEV(0)
#endif
#ifndef LPS331AP_PARAM_ADDR
#define LPS331AP_PARAM_ADDR             (0x44)
#endif
#ifndef LPS331AP_PARAM_RATE
#define LPS331AP_PARAM_RATE             (LPS331AP_RATE_7HZ)
#endif

#ifndef LPS331AP_PARAMS
#define LPS331AP_PARAMS                 { .i2c  = LPS331AP_PARAM_I2C,  \
                                          .addr = LPS331AP_PARAM_ADDR, \
                                          .rate = LPS331AP_PARAM_RATE }
#endif
#ifndef LPS331AP_SAUL_INFO
#define LPS331AP_SAUL_INFO              { .name = "lps331ap" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lps331ap_params_t lps331ap_params[] =
{
    LPS331AP_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lps331ap_saul_info[] =
{
    LPS331AP_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* LPS331AP_PARAMS_H */
/** @} */
