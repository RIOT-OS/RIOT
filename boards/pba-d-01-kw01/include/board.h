/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_pba-d-01-kw01 phyWAVE-KW01 Evalboard
 * @ingroup     boards
 * @brief       Board specific implementations for the phyWAVE-KW01 evaluation board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the phyWAVE-KW01 evaluation board
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_RX_BUFSIZE    (64U)
#define STDIO_BAUDRATE      (115200U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_R_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK)) /**< Clock Enable for PORTD*/
#define LED_G_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK)) /**< Clock Enable for PORTD*/
#define LED_B_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK)) /**< Clock Enable for PORTA*/
#define LED_R_PORT            PORTA /**< PORT for Red LED */
#define LED_R_GPIO            GPIOA /**< GPIO-Device for Red LED */
#define LED_G_PORT            PORTB /**< PORT for Green LED */
#define LED_G_GPIO            GPIOB /**< GPIO-Device for Green LED */
#define LED_B_PORT            PORTC /**< PORT for Blue LED */
#define LED_B_GPIO            GPIOC /**< GPIO-Device for Blue LED */
#define LED_R_PIN             4	    /**< Red LED connected to PINx */
#define LED_G_PIN             2	    /**< Green LED connected to PINx */
#define LED_B_PIN             3	    /**< Blue LED connected to PINx */
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_B_ON            (LED_B_GPIO->PCOR |= (1 << LED_B_PIN))
#define LED_B_OFF           (LED_B_GPIO->PSOR |= (1 << LED_B_PIN))
#define LED_B_TOGGLE        (LED_B_GPIO->PTOR |= (1 << LED_B_PIN))
#define LED_G_ON            (LED_G_GPIO->PCOR |= (1 << LED_G_PIN))
#define LED_G_OFF           (LED_G_GPIO->PSOR |= (1 << LED_G_PIN))
#define LED_G_TOGGLE        (LED_G_GPIO->PTOR |= (1 << LED_G_PIN))
#define LED_R_ON            (LED_R_GPIO->PCOR |= (1 << LED_R_PIN))
#define LED_R_OFF           (LED_R_GPIO->PSOR |= (1 << LED_R_PIN))
#define LED_R_TOGGLE        (LED_R_GPIO->PTOR |= (1 << LED_R_PIN))

/* for compatability to other boards */
#define LED_GREEN_ON        LED_G_ON
#define LED_GREEN_OFF       LED_G_OFF
#define LED_GREEN_TOGGLE    LED_G_TOGGLE
#define LED_RED_ON          LED_R_ON
#define LED_RED_OFF         LED_R_OFF
#define LED_RED_TOGGLE      LED_R_TOGGLE
/** @} */

/**
 * Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
