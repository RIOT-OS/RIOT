/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_itg320x
 * @brief       Default configuration for InvenSense ITG320X 3-axis gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "board.h"
#include "itg320x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef ITG320X_PARAM_DEV
#define ITG320X_PARAM_DEV          I2C_DEV(0)
#endif
#ifndef ITG320X_PARAM_ADDR
#define ITG320X_PARAM_ADDR         (ITG320X_I2C_ADDRESS_1)
#endif
#ifndef ITG320X_PARAM_LPF_BW
#define ITG320X_PARAM_LPF_BW       (ITG320X_LPF_BW_5)
#endif
#ifndef ITG320X_PARAM_ISR_DIV
#define ITG320X_PARAM_ISR_DIV      (99)
#endif
#ifndef ITG320X_PARAM_CLK_SEL
#define ITG320X_PARAM_CLK_SEL      (ITG320X_CLK_PLL_X_GYRO)
#endif
#ifndef ITG320X_PARAM_INT_PIN
#define ITG320X_PARAM_INT_PIN      (GPIO_UNDEF)
#endif
#ifndef ITG320X_PARAM_INT_LEVEL
#define ITG320X_PARAM_INT_LEVEL    (ITG320X_INT_LOW)
#endif
#ifndef ITG320X_PARAM_INT_DRIVE
#define ITG320X_PARAM_INT_DRIVE    (ITG320X_INT_PUSH_PULL)
#endif

#ifndef ITG320X_PARAMS
#ifdef MODULE_ITG320X_INT
#define ITG320X_PARAMS             { \
                                     .dev  = ITG320X_PARAM_DEV,  \
                                     .addr = ITG320X_PARAM_ADDR, \
                                     .lpf_bw  = ITG320X_PARAM_LPF_BW,  \
                                     .isr_div = ITG320X_PARAM_ISR_DIV, \
                                     .clk_sel = ITG320X_PARAM_CLK_SEL, \
                                     .int_pin = ITG320X_PARAM_INT_PIN, \
                                     .int_level = ITG320X_PARAM_INT_LEVEL, \
                                     .int_drive = ITG320X_PARAM_INT_DRIVE, \
                                   }
#else /* MODULE_ITG320X_INT */
#define ITG320X_PARAMS             { \
                                     .dev  = ITG320X_PARAM_DEV,  \
                                     .addr = ITG320X_PARAM_ADDR, \
                                     .lpf_bw  = ITG320X_PARAM_LPF_BW,  \
                                     .isr_div = ITG320X_PARAM_ISR_DIV, \
                                     .clk_sel = ITG320X_PARAM_CLK_SEL, \
                                   }
#endif /* MODULE_ITG320X_INT */
#endif /* ITG320X_PARAMS */

#ifndef ITG320X_SAUL_INFO
#define ITG320X_SAUL_INFO          { .name = "itg320x" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const itg320x_params_t itg320x_params[] =
{
    ITG320X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t itg320x_saul_info[] =
{
    ITG320X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
