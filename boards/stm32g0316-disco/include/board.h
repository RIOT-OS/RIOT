/*
 * Copyright (C) 2021 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32g0316-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32G0316-DISCO
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LED0_PIN            GPIO_PIN(PORT_A, 12)
#define LED0_MODE           GPIO_OUT
#define LED0_MASK           (1 << 12)

#define LED0_ON             (GPIOA->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOA->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOA->ODR  ^= LED0_MASK)

#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
