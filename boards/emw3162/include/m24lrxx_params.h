/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_emw3162
 * @{
 *
 * @file
 * @brief     M24LR04E board specific configuration
 *
 * @author    N v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef M24LRXXX_PARAMS_H
#define M24LRXXX_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    M24LR04E configuration
 */
static const  m24lrxxx_params_t m24lrxxx_params[] =
{
    {
        .bus   = M24LRXXX_I2C,
        .speed = I2C_SPEED_FAST,
        .address = 0xa6
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t m24lrxxx_saul_info[] =
{
    {
        .name = "m24lr04e",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* M24LRXXX_PARAMS_H */
/** @} */
