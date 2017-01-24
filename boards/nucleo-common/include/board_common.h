/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo_common STM Nucleo Common
 * @ingroup     drivers_periph
 * @brief       Common files for STM Nucleo boards
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_COMMON__H
#define BOARD_COMMON__H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A, 5)

#define LED0_MASK           (1 << 5)

#if defined(CPU_FAM_STM32F4)
#define LED_CREG            BSRRH
#else
#define LED_CREG            BRR
#endif
#if defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1)
#define LED_SREG            BSRRL
#else
#define LED_SREG            BSRR
#endif

#define LED0_ON             (GPIOA->LED_SREG = LED0_MASK)
#define LED0_OFF            (GPIOA->LED_CREG = LED0_MASK)
#define LED0_TOGGLE         (GPIOA->ODR     ^= LED0_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_C, 13)

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON__H */
/** @} */
