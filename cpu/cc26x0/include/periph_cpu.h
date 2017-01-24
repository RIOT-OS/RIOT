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
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

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
#endif /* ndef DOXYGEN */

/**
 * @brief   Timer configuration options
 */
typedef struct {
    gpt_reg_t *dev; /**< the GPT base address */
    uint8_t num; /**< number of the timer */
    uint8_t irqn; /**< interrupt number */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
