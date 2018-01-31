/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_jiminy-mega256rfr2 Jiminy- Mega256rfr2
 * @ingroup     boards
 * @brief       Board specific files for the Jiminy Mega 256rfr2 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Jiminy Mega 256rfr2 board.
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/*Fastest Baudrate possible for board*/
#define UART_STDIO_BAUDRATE (38400)       /**< Sets Baudrate for e.g. Shell */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORTB
#define LED_PORT_DDR        DDRB

#define LED0_PIN            GPIO_PIN(1, 5)
#define LED1_PIN            GPIO_PIN(1, 6)
#define LED2_PIN            GPIO_PIN(1, 7)

#define LED0_MASK           (1 << DDB5)
#define LED1_MASK           (1 << DDB6)
#define LED2_MASK           (1 << DDB7)

#define LED0_ON             (LED_PORT |=  LED0_MASK)
#define LED0_OFF            (LED_PORT &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT ^=  LED0_MASK)

#define LED1_ON             (LED_PORT |=  LED1_MASK)
#define LED1_OFF            (LED_PORT &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT ^=  LED1_MASK)

#define LED2_ON             (LED_PORT |=  LED2_MASK)
#define LED2_OFF            (LED_PORT &= ~LED2_MASK)
#define LED2_TOGGLE         (LED_PORT ^=  LED2_MASK)
/** @} */

/**
 * @name Context swap defines
 * This emulates a software triggered interrupt
 * @{
 */
#define AVR_CONTEXT_SWAP_INIT do { \
        DDRE |= (1 << PE7); \
        EICRB |= (1 << ISC70); \
        EIMSK |= (1 << INT7); \
        sei(); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  INT7_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTE ^= (1 << PE7)
/** @} */


/**
 * @name Initialize board specific hardware, including clock, LEDs and std-IO
 * @{
 */
void board_init(void);
/** @} */
#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
