/*
 * Copyright (C) 2019 University of Applied Sciences Emden / Leer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ph_oem
 * @{
 *
 * @file
 * @brief       Default configuration for Atlas Scientific pH OEM sensors
 *
 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 */


#include "board.h" /* THIS INCLUDE IS MANDATORY */
#include "saul_reg.h"
#include "ph_oem.h"
#include "ph_oem_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Atlas Scientific pH OEM driver
 * @{
 */
#ifndef PH_OEM_PARAM_I2C
#define PH_OEM_PARAM_I2C                  (I2C_DEV(0))
#endif
#ifndef PH_OEM_PARAM_ADDR
#define PH_OEM_PARAM_ADDR                 (0x65)
#endif
#ifndef PH_OEM_PARAM_INTERRUPT_PIN
#define PH_OEM_PARAM_INTERRUPT_PIN        (GPIO_UNDEF)
#endif
#ifndef PH_OEM_PARAM_INTERRUPT_OPTION
#define PH_OEM_PARAM_INTERRUPT_OPTION     (PH_OEM_IRQ_BOTH)
#endif
#ifndef PH_OEM_PARAM_INTERRUPT_GPIO_MODE
#define PH_OEM_PARAM_INTERRUPT_GPIO_MODE  (GPIO_IN_PD)
#endif

#ifndef PH_OEM_PARAMS
#define PH_OEM_PARAMS       { .i2c = PH_OEM_PARAM_I2C,        \
                              .addr = PH_OEM_PARAM_ADDR,       \
                              .interrupt_pin = PH_OEM_PARAM_INTERRUPT_PIN, \
                              .gpio_mode = PH_OEM_PARAM_INTERRUPT_GPIO_MODE, \
                              .irq_option = PH_OEM_PARAM_INTERRUPT_OPTION }
#endif
#ifndef PH_OEM_SAUL_INFO
#define PH_OEM_SAUL_INFO       { .name = "pH OEM sensor" }
#endif
/** @} */
/**
 * @brief   pH OEM defaults if not defined for a board or application
 */
static const ph_oem_params_t ph_oem_params[] =
{
    PH_OEM_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t ph_oem_saul_info[] =
{
    PH_OEM_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
