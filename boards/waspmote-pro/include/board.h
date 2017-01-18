/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_waspmote-pro Waspmote PRO v1.2
 * @ingroup     boards
 * @brief       Board specific files for the Waspmote PRO v1.2 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Waspmote PRO v1.2 board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "waspmote_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief As the CPU is too slow to handle 115200 baud, we set the default
*        baudrate to 9600 for this board
*/
#define UART_STDIO_BAUDRATE  (9600U)
/** @} */

/**
 * @brief Use the UART 0 for STDIO on this board, if the XBee socket is not
 *        being used
 */
#ifdef XBEE_UART
#if XBEE_UART == 0
#define UART_STDIO_DEV       (UART_DEV(1))
#else
#define UART_STDIO_DEV       (UART_DEV(0))
#endif
#endif

/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED0_PORT            PORTD
#define LED1_PORT            PORTC
#define LED0_PIN             (1 << 6)
#define LED1_PIN             (1 << 1)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_ENABLE_PORT     DDRD |= (1 << DDD6)
#define LED0_ON              LED0_PORT |= LED0_PIN
#define LED0_OFF             LED0_PORT &= ~LED0_PIN
#define LED0_TOGGLE          LED0_PORT ^= LED0_PIN;

#define LED1_ENABLE_PORT     DDRC |= (1 << DDC1)
#define LED1_ON              LED1_PORT |= LED1_PIN
#define LED1_OFF             LED1_PORT &= ~LED1_PIN
#define LED1_TOGGLE          LED1_PORT ^= LED1_PIN;

/* for compatibility to other boards */
#define LED_GREEN_ON         LED1_ON
#define LED_GREEN_OFF        LED1_OFF
#define LED_GREEN_TOGGLE     LED1_TOGGLE
#define LED_RED_ON           LED0_ON
#define LED_RED_OFF          LED0_OFF
#define LED_RED_TOGGLE       LED0_TOGGLE
/** @} */

/**
 * @name Macros for controlling the on-board MUXes.
 * @{
 */
#define MUX_PW_PORT                  PORTD
#define MUX0_PORT                    PORTB
#define MUX1_PORT                    PORTB
#define MUX_USB_XBEE_PORT            PORTD
#define MUX_PW_PIN                   (1 << 7)
#define MUX0_PIN                     (1 << 6)
#define MUX1_PIN                     (1 << 7)
#define MUX_USB_XBEE_PIN             (1 << 5)

#define MUX_PW_ENABLE_PORT           DDRD |= (1 << DDD7);
#define MUX_PW_ON                    MUX_PW_PORT |= MUX_PW_PIN
#define MUX_PW_OFF                   MUX_PW_PORT &= ~MUX_PW_PIN

#define MUX0_ENABLE_PORT             DDRB |= (1 << DDB6)
#define MUX0_ON                      MUX0_PORT |= MUX0_PIN
#define MUX0_OFF                     MUX0_PORT &= ~MUX0_PIN

#define MUX1_ENABLE_PORT             DDRB |= (1 << DDB7)
#define MUX1_ON                      MUX1_PORT |= MUX1_PIN
#define MUX1_OFF                     MUX1_PORT &= ~MUX1_PIN

#define MUX_USB_XBEE_ENABLE_PORT     DDRD |= (1 << DDD5)
#define MUX_USB_XBEE_ON              MUX_USB_XBEE_PORT |= MUX_USB_XBEE_PIN
#define MUX_USB_XBEE_OFF             MUX_USB_XBEE_PORT &= ~MUX_USB_XBEE_PIN

/* Multiplexer settings to enable UART1 on the desired module
 *
 * --------------
 * MUX0_OFF & MUX1_ON  ---> GPS MODULE
 * MUX0_ON  & MUX1_ON  ---> SOCKET1
 * MUX0_ON  & MUX1_OFF ---> AUX1 MODULE
 * MUX0_OFF & MUX1_OFF ---> AUX2 MODULE
 *
 * Multiplexer setting to enable UART0 to the desired module
 *
 * --------------
 * MUX_USB_XBEE_OFF    ---> USB MODULE
 * MUX_USB_XBEE_ON     ---> SOCKET0
 *
 */

#define SET_MUX_GPS                  MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX0_ENABLE_PORT; MUX1_ENABLE_PORT; \
                                     MUX0_OFF; MUX1_ON
#define SET_MUX_SOCKET1              MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX0_ENABLE_PORT; MUX1_ENABLE_PORT; \
                                     MUX0_ON; MUX1_ON
#define SET_MUX_AUX1_MODULE          MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX0_ENABLE_PORT; MUX1_ENABLE_PORT; \
                                     MUX0_ON; MUX1_OFF
#define SET_MUX_AUX2_MODULE          MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX0_ENABLE_PORT; MUX1_ENABLE_PORT; \
                                     MUX0_OFF; MUX1_OFF
#define SET_MUX_USB_MODULE           MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX_USB_XBEE_ENABLE_PORT; \
                                     MUX_USB_XBEE_OFF
#define SET_MUX_SOCKET0              MUX_PW_ENABLE_PORT; MUX_PW_ON; \
                                     MUX_USB_XBEE_ENABLE_PORT; \
                                     MUX_USB_XBEE_ON


/** @} */

/**
 * Context swap defines
 * Setup to use PB5 which is pin change interrupt 5
 * This emulates a software triggered interrupt
 **/
#define AVR_CONTEXT_SWAP_INIT do { \
    DDRB |= (1 << PB5); \
    PCICR |= (1 << PCIE0); \
    PCMSK0 |= (1 << PCINT5); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  PCINT0_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTB ^= (1 << PB5)

/**
 * @brief xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_SHIFT                (4)
#define XTIMER_HZ                   (62500UL)
#define XTIMER_BACKOFF              (40)
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
