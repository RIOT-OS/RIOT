/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_zynq7000
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __PERIPH_CPU_H
#define __PERIPH_CPU_H

#include "cpu.h"
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_GPIO_T
typedef uint32_t gpio_t;

/**
 * @brief   Available GPIO ports
 */
enum {
    BANK0 = 0,
    BANK1 = 1,
    BANK2 = 2,
	BANK3 = 3,
};

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING,
	GPIO_FALLING,
	GPIO_BOTH,
	GPIO_LEVEL_HIGH,
	GPIO_LEVEL_LOW
} gpio_flank_t;
/** @} */

/**
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO(x,y)       ((x & 0) | y)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CPU_H */
/** @} */
