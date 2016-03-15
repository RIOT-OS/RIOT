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
 * @brief     SHT11 board specific configuration
 *
 * @author    N v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef SHT11_PARAMS_H
#define SHT11_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    SHT11 configuration
 */
static const  sht11_params_t sht11_params[] =
{
    {
        .bus   = SHT11_I2C,
        .speed = I2C_SPEED_NORMAL
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t sht11_saul_info[] =
{
    {
        .name = "sht11",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* SHT11_PARAMS_H */
/** @} */
