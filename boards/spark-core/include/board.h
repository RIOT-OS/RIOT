/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_spark-core Spark-Core
 * @ingroup     boards
 * @brief       Board specific files for the spark-core board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the spark-core board.
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Define the location of the RIOT image in flash
 */
#define LOCATION_VTABLE     (0x08005000)

/**
 * @name Tell the xtimer that we use a 16-bit peripheral timer
 */
#define XTIMER_WIDTH        (16)

/**
 * @brief   Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A, 9)
#define LED1_PIN            GPIO_PIN(PORT_A, 10)
#define LED2_PIN            GPIO_PIN(PORT_A, 8)
#define LED3_PIN            GPIO_PIN(PORT_A, 13)

#define LED_PORT            (GPIOA)
#define LED0_MASK           (1 << 9)
#define LED1_MASK           (1 << 10)
#define LED2_MASK           (1 << 8)
#define LED3_MASK           (1 << 13)

#define LED0_ON             (LED_PORT->BRR  = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRR = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->ODR ^= LED0_MASK)

#define LED1_ON             (LED_PORT->BRR  = LED1_MASK)
#define LED1_OFF            (LED_PORT->BSRR = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->ODR ^= LED1_MASK)

#define LED2_ON             (LED_PORT->BRR  = LED2_MASK)
#define LED2_OFF            (LED_PORT->BSRR = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->ODR ^= LED2_MASK)

#define LED3_ON             (LED_PORT->BRR  = LED3_MASK)
#define LED3_OFF            (LED_PORT->BSRR = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->ODR ^= LED3_MASK)
/** @} */

/**
 * @name User button configuration
 */
#define BUTTON1             GPIO_PIN(PORT_B,2)

/**
 * @name CC3000 pin configuration
 * @{
 */
#define CC3000_SPI          SPI_DEV(0)
#define CC3000_CS           GPIO_PIN(PORT_B,12)
#define CC3000_EN           GPIO_PIN(PORT_B,8)
#define CC3000_INT          GPIO_PIN(PORT_B,11)
/** @} */

/**
 * @name EXTFLASH pin configuration
 * @{
 */
#define EXTFLASH_SPI        SPI_DEV(0)
#define EXTFLASH            GPIO_PIN(PORT_B,9)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
