/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Leon M. George <leon@georgemail.eu>
 * @author          Anton Gerasimov <tossel@gmail.com>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifndef SET_MODE_CONF_1_ALT_DCDC_IPEAK
#define SET_MODE_CONF_1_ALT_DCDC_IPEAK 0x2 /**< 12 mA */
#endif

#ifndef SET_MODE_CONF_XOSC_FREQ
#define SET_MODE_CONF_XOSC_FREQ        0x2 /**< 48 MHz xtal */
#endif

#define __FPU_PRESENT                  1U  /**< Presence of FPU */
#include "cpu_conf_cc26xx_cc13xx.h"

#include "cc26x2_cc13x2_aux.h"
#include "cc26x2_cc13x2_fcfg.h"
#include "cc26x2_cc13x2_prcm.h"
#include "cc26x2_cc13x2_setup.h"

/**
 * @brief   Bit-Band configuration
 */
#define CPU_HAS_BITBAND 1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CONF_H */
