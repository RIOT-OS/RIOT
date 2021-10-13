/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26x0_cc13x0
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
#define SET_MODE_CONF_1_ALT_DCDC_IPEAK 0x2 /**< 32 mA */
#endif

#ifndef SET_MODE_CONF_XOSC_FREQ
#define SET_MODE_CONF_XOSC_FREQ        0x3 /**< 24 MHz xtal */
#endif

#include "cpu_conf_cc26xx_cc13xx.h"

#include "cc26x0_cc13x0_aux.h"
#include "cc26x0_cc13x0_fcfg.h"
#include "cc26x0_cc13x0_prcm.h"

/**
 * @brief   Bit-Band configuration
 */
#define CPU_HAS_BITBAND 1

#ifdef __cplusplus
extern "C" {
#endif

/* Empty, but makes linter happy */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
