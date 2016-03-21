/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief     tmp006 board specific configuration
 *
 * @author    Johann Fischer <j.fischer@phytec.de>
 */

#ifndef TMP006_PARAMS_H
#define TMP006_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    tmp006 parameter configuration
 */
static const  tmp006_params_t tmp006_params[] =
{
    {
        .i2c = TMP006_I2C,
        .addr = TMP006_ADDR,
        .ratio = TMP006_CONFIG_CR_DEF,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tmp006_saul_info[] =
{
    {
        .name = "tmp006",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* TMP006_PARAMS_H */
/** @} */
