/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hts221
 *
 * @{
 * @file
 * @brief       Default configuration for ST HTS221 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef HTS221_PARAMS_H
#define HTS221_PARAMS_H

#include "board.h"
#include "hts221.h"
#include "hts221_regs.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Set default configuration parameters for the HTS221 driver
 * @{
 */
#ifndef HTS221_PARAM_I2C
#define HTS221_PARAM_I2C    I2C_DEV(0)
#endif
#ifndef HTS221_PARAM_ADDR
#define HTS221_PARAM_ADDR   (HTS221_I2C_ADDRESS)
#endif
#ifndef HTS221_PARAM_AVGX
#define HTS221_PARAM_AVGX   ((HTS221_REGS_AVGT_16 << HTS221_REGS_AVGT_SHIFT) | \
                             HTS221_REGS_AVGH_32)
#endif
#ifndef HTS221_PARAM_RATE
#define HTS221_PARAM_RATE   (HTS221_REGS_CTRL_REG1_ODR_12HZ)
#endif
#ifndef HTS221_PARAMS
#define HTS221_PARAMS       { .i2c  = HTS221_PARAM_I2C, \
                              .addr = HTS221_PARAM_ADDR, \
                              .avgx = HTS221_PARAM_AVGX, \
                              .rate = HTS221_PARAM_RATE }
#endif /* HTS221_PARAMS */

#ifndef HTS221_SAULINFO
#define HTS221_SAULINFO     { .name = "hts221" }
#endif
/**@}*/

/**
 * @brief   HTS221 configuration
 */
static const hts221_params_t hts221_params[] =
{
    HTS221_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hts221_saul_info[] =
{
    HTS221_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HTS221_PARAMS_H */
