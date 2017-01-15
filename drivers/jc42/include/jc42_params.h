/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_jc42
 *
 * @{
 * @file
 * @brief       Default configuration for jc42
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef JC42_PARAMS_H
#define JC42_PARAMS_H

#include "jc42.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the JC42
 * @{
 */
#ifndef JC42_PARAM_I2C_DEV
#define JC42_PARAM_I2C_DEV         (0)
#endif
#ifndef JC42_PARAM_ADDR
#define JC42_PARAM_ADDR            (0x18)
#endif
#ifndef JC42_PARAM_SPEED
#define JC42_PARAM_SPEED           I2C_SPEED_FAST
#endif

#define JC42_PARAMS_DEFAULT        {.i2c = JC42_PARAM_I2C_DEV,  \
                                    .speed    = JC42_PARAM_SPEED, \
                                    .addr     = JC42_PARAM_ADDR }
/**@}*/

/**
 * @brief   Configure JC42
 */
static const jc42_params_t jc42_params[] =
{
#ifdef JC42_PARAMS_BOARD
    JC42_PARAMS_BOARD,
#else
    JC42_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* JC42_PARAMS_H */
/** @} */
