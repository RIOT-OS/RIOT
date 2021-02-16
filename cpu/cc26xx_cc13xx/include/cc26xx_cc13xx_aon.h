/*
 * Copyright (C) 2021 Jean Pierre Dudey
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU I/O register definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_AON_H
#define CC26XX_CC13XX_AON_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AON_BATMON registers
 */
typedef struct {
    reg32_t CTL; /**< Internal */
    reg32_t MEASCFG; /**< Internal */
    reg32_t TEMPP0; /**< Internal */
    reg32_t TEMPP1; /**< Internal */
    reg32_t TEMPP2; /**< Internal */
    reg32_t BATMONP0; /**< Internal */
    reg32_t BATMONP1; /**< Internal */
    reg32_t IOSTRP0; /**< Internal */
    reg32_t FLASHPUMPP0; /**< Internal */
    reg32_t BAT; /**< Last Measured Battery Voltage */
    reg32_t BATUPD; /**< Battery Update */
    reg32_t TEMP; /**< Temperature */
    reg32_t TEMPUPD; /**< Temperature Update */
    reg32_t __reserved1; /**< Reserved */
    reg32_t __reserved2; /**< Reserved */
    reg32_t __reserved3; /**< Reserved */
#ifdef CPU_VARIANT_X2
    reg32_t EVENTMASK; /**< Event Mask */
    reg32_t EVENT; /**< Event */
    reg32_t BATTUL; /**< Battery Upper Limit */
    reg32_t BATTLL; /**< Battery Lower Limit */
    reg32_t TEMPUL; /**< Temperature Upper Limit */
    reg32_t TEMPLL; /**< Temperature Lower Limit */
#endif
} aon_batmon_regs_t;

/**
 * @brief   AON_BATMON register values
 * @{
 */
#define AON_BATMON_CTL_CALC_EN                  0x00000002
#define AON_BATMON_CTL_MEAS_EN                  0x00000001
#define AON_BATMON_BAT_FRAC_s                   0
#define AON_BATMON_TEMP_INT_w                   9
#define AON_BATMON_TEMP_INT_s                   8
#define AON_BATMON_EVENTMASK_TEMP_BELOW_LL_MASK 0x00000008
#define AON_BATMON_EVENTMASK_TEMP_OVER_UL_MASK  0x00000004
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_BATMON_BASE             (PERIPH_BASE + 0x95000)
/** @} */

/**
 * @brief   AON_BATMON register bank
 */
#define AON_BATMON                  ((aon_batmon_regs_t *) (AON_BATMON_BASE))

#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_AON_H */
/** @} */
