/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wstk6220 Silicon Labs WSTK6220 Eval Kit
 * @ingroup     boards
 * @brief       Support for the Silicon Labs WSTK6220 evaluation kit
 * @{
 *
 * @file
 * @brief       Board specific definitions for the WSTK6220 evaluation kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the nominal CPU core clock in this board
 */
#define F_CPU               (CLOCK_CORECLOCK)

/**
 * @brief   Assign the hardware timer
 */
#define HW_TIMER            TIMER_DEV(0)

/**
 * @brief   Define UART device and baudrate for STDIO
 * @{
 */
#define STDIO               UART_DEV(0)
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @brief   Connection to the on-board temperature/humidity sensor (Si7021)
 * @{
 */
#define SI7021_I2C          (I2C_0)
#define SI7021_ADDR         (0)     /* TODO */
#define SI7021_EN_PIN       GPIO_PIN(PF,8)
/** @} */

/**
 * @brief   GPIO pin for enabling communication through the board controller
 */
#define BC_PIN              GPIO_PIN(PA,12)

/**
 * @brief   User button pin definitions
 * @{
 */
#define PB0_PIN             GPIO_PIN(PE,3)
#define PB1_PIN             GPIO_PIN(PE,2)
/** @} */

/**
 * @brief    LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PF,6)
#define LED1_PIN            GPIO_PIN(PF,7)
/** @} */

/**
 * @brief   Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)
#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)

/* for compatability to other boards */
#define LED_GREEN_ON        LED1_ON
#define LED_GREEN_OFF       LED1_OFF
#define LED_GREEN_TOGGLE    LED1_TOGGLE
#define LED_ORANGE_ON       /* not available */
#define LED_ORANGE_OFF      /* not available */
#define LED_ORANGE_TOGGLE   /* not available */
#define LED_RED_ON          LED0_ON
#define LED_RED_OFF         LED0_OFF
#define LED_RED_TOGGLE      LED0_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
