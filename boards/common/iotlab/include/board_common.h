/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_common_iotlab IoTlab Common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for the IoTlab boards.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the iotlab-m3 board.
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set the default baudrate to 500K for this board
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#   define STDIO_UART_BAUDRATE (500000U)
#endif
/** @} */

/**
 * @name    Xtimer configuration
 *
 * Tell the xtimer that we use a 16-bit peripheral timer
 * @{
 */
#define XTIMER_WIDTH        (16U)
#define XTIMER_OVERHEAD     (6U)
#define XTIMER_SHOOT_EARLY  (3U)
/** @} */

/**
 * @name    Define the interface to the AT86RF231 radio
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 * @{
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PORT_A, 4)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PORT_C, 4)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PORT_A, 2)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PORT_C, 1)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_D, 2)
#define LED1_PIN            GPIO_PIN(PORT_B, 5)
#define LED2_PIN            GPIO_PIN(PORT_C, 10)

#define LED0_MASK           (1 << 2)
#define LED1_MASK           (1 << 5)
#define LED2_MASK           (1 << 10)

#define LED0_ON             (GPIOD->ODR &= ~LED0_MASK)
#define LED0_OFF            (GPIOD->ODR |=  LED0_MASK)
#define LED0_TOGGLE         (GPIOD->ODR ^=  LED0_MASK)

#define LED1_ON             (GPIOB->ODR &= ~LED1_MASK)
#define LED1_OFF            (GPIOB->ODR |=  LED1_MASK)
#define LED1_TOGGLE         (GPIOB->ODR ^=  LED1_MASK)

#define LED2_ON             (GPIOC->ODR &= ~LED2_MASK)
#define LED2_OFF            (GPIOC->ODR |=  LED2_MASK)
#define LED2_TOGGLE         (GPIOC->ODR ^=  LED2_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
