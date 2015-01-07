/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_mbed_lpc1768 mbed LPC1768 development kit
 * @ingroup     boards
 * @brief       Support for the mbed LPC1762 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the mbed_lpc1768 board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "bitarithm.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The nominal CPU core clock in this board
 */
#define F_CPU               (96000000)

/**
 * @brief Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            LPC_GPIO1
#define LED1_PIN            BIT18
#define LED2_PIN            BIT20
#define LED3_PIN            BIT21
#define LED4_PIN            BIT23
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED1_ON              (LED_PORT->FIOSET = LED1_PIN)
#define LED1_OFF             (LED_PORT->FIOCLR = LED1_PIN)
#define LED1_TOGGLE          (LED_PORT->FIOPIN ^= LED1_PIN)
#define LED2_ON              (LED_PORT->FIOSET = LED2_PIN)
#define LED2_OFF             (LED_PORT->FIOCLR = LED2_PIN)
#define LED2_TOGGLE          (LED_PORT->FIOPIN ^= LED2_PIN)
#define LED3_ON              (LED_PORT->FIOSET = LED3_PIN)
#define LED3_OFF             (LED_PORT->FIOCLR = LED3_PIN)
#define LED3_TOGGLE          (LED_PORT->FIOPIN ^= LED3_PIN)
#define LED4_ON              (LED_PORT->FIOSET = LED4_PIN)
#define LED4_OFF             (LED_PORT->FIOCLR = LED4_PIN)
#define LED4_TOGGLE          (LED_PORT->FIOPIN ^= LED4_PIN)

/* for compatibility to other boards */
#define LED_GREEN_ON        LED1_ON
#define LED_GREEN_OFF       LED1_OFF
#define LED_GREEN_TOGGLE    LED1_TOGGLE
#define LED_RED_ON          LED4_ON
#define LED_RED_OFF         LED4_OFF
#define LED_RED_TOGGLE      LED4_TOGGLE
/** @} */

/**
 * @brief define radio packet length
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, include clocks, LEDs and stdio
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
/** @} */
