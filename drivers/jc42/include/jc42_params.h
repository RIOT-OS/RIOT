/*
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_jc42
 *
 * @{
 * @file
 * @brief       Default configuration for jc42
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"
#include "jc42.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the JC42
 * @{
 */
#ifndef JC42_PARAM_I2C_DEV
#define JC42_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef JC42_PARAM_ADDR
#define JC42_PARAM_ADDR            (0x18)
#endif
#ifndef JC42_PARAM_SPEED
#define JC42_PARAM_SPEED           I2C_SPEED_FAST
#endif

#ifndef JC42_PARAMS
#define JC42_PARAMS                { .i2c   = JC42_PARAM_I2C_DEV, \
                                     .speed = JC42_PARAM_SPEED,   \
                                     .addr  = JC42_PARAM_ADDR }
#endif
#ifndef JC42_SAUL_INFO
#define JC42_SAUL_INFO             { .name= "jc42" }
#endif
/**@}*/

/**
 * @brief   Configure JC42
 */
static const jc42_params_t jc42_params[] =
{
    JC42_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t jc42_saul_info[] =
{
    JC42_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
