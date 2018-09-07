/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Leon M. George <leon@georgemail.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting offset of CPU_ID
 */
#define CPUID_ADDR          (&FCFG->MAC_BLE_0)
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST_CORTEXM
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode values
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_OFF),      /**< input w/o pull R */
    GPIO_IN_PD = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_DOWN),     /**< input with pull-down */
    GPIO_IN_PU = (IOCFG_INPUT_ENABLE | IOCFG_PULLCTL_UP),       /**< input with pull-up */
    GPIO_OUT   = (IOCFG_PULLCTL_OFF),                           /**< push-pull output */
    GPIO_OD    = (IOCFG_IOMODE_OPEN_DRAIN | IOCFG_PULLCTL_OFF), /**< open-drain w/o pull R */
    GPIO_OD_PU = (IOCFG_IOMODE_OPEN_DRAIN | IOCFG_PULLCTL_UP)   /**< open-drain with pull-up */
} gpio_mode_t;

/**
 * @brief   Override GPIO flank values
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = IOCFG_EDGEDET_FALLING,
    GPIO_RISING = IOCFG_EDGEDET_RISING,
    GPIO_BOTH = IOCFG_EDGEDET_BOTH
} gpio_flank_t;

/**
 * @brief   Configuration of low-level general purpose timers
 *
 * General purpose timers (GPT[0-3]) are configured consecutively and in order
 * (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
 */
typedef struct {
    uint8_t     cfg;    /**< timer config [16,32 Bit] */
    uint8_t     chn;    /**< number of channels [1,2] */
} timer_conf_t;

/**
 * @brief Timer implementation provides custom timer_diff function
 */
#define PERIPH_TIMER_PROVIDES_DIFF

#endif /* ifndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
