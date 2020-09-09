/*
 * Copyright (C) 2020 Nalys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bh1900nux
 *
 * @{
 * @file
 * @brief       Default configuration for BH1900NUX devices
 *
 * @author      Wouter Symons <wsymons@nalys-group.com>
 */

#ifndef BH1900NUX_PARAMS_H
#define BH1900NUX_PARAMS_H

#include "board.h"
#include "bh1900nux.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for BH1900NUX devices
 * @{
 */
#ifndef BH1900NUX_PARAM_I2C
#define BH1900NUX_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef BH1900NUX_PARAM_ADDR
#define BH1900NUX_PARAM_ADDR        (BH1900NUX_DEFAULT_ADDR)
#endif

#ifndef BH1900NUX_PARAMS
#define BH1900NUX_PARAMS            { .i2c = BH1900NUX_PARAM_I2C,   \
                                      .addr = BH1900NUX_PARAM_ADDR }
#endif
/**@}*/

/**
 * @brief   BH1900NUX configuration
 */
static const bh1900nux_params_t bh1900nux_params[] =
{
    BH1900NUX_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BH1900NUX_PARAMS_H */
/** @} */
