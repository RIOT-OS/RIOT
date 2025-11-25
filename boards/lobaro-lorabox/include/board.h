/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lobaro_lorabox
 * @brief       Support for Lobaro LoraBox
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        1
#define LED0_PORT_NUM       PORT_A
#define LED0_IS_INVERTED    1

#define EN3V3_PORT          GPIOA
#define EN3V3_PIN           GPIO_PIN(PORT_A, 11)
#define EN3V3_MASK          (1 << 11)

#define EN3V3_ON            (EN3V3_PORT->BSRR = EN3V3_MASK)
#define EN3V3_OFF           (EN3V3_PORT->BSRR = (EN3V3_MASK << 16))
#define EN3V3_TOGGLE        (EN3V3_PORT->ODR  ^= EN3V3_MASK)
/** @} */

/**
 * @name        SX127X
 *
 * SX127X configuration.
 * @{
 */
#define SX127X_PARAM_SPI_NSS            GPIO_PIN(PORT_B, 0)
#define SX127X_PARAM_RESET              GPIO_PIN(PORT_A, 4)
#define SX127X_PARAM_DIO0               GPIO_PIN(PORT_B, 1)
#define SX127X_PARAM_DIO1               GPIO_PIN(PORT_B, 10)
#define SX127X_PARAM_DIO2               GPIO_PIN(PORT_B, 11)
#define SX127X_PARAM_DIO3               GPIO_PIN(PORT_B, 7)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
