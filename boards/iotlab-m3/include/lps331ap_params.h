/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_iotlab-m3
 * @{
 *
 * @file
 * @brief     LPS331AP board specific configuration
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LPS331AP_PARAMS_H
#define LPS331AP_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LPS331AP configuration
 */
static const  lps331ap_params_t lps331ap_params[] =
{
    {
        .i2c = LPS331AP_I2C,
        .addr = LPS331AP_ADDR,
        .rate = LPS331AP_RATE_7HZ,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lps331ap_saul_info[] =
{
    {
        .name = "lps331ap",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* LPS331AP_PARAMS_H */
/** @} */
