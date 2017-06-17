/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_pinoccio-mega256rfr2 Pinoccio Mega 256rfr2
 * @ingroup     boards
 * @brief       Board specific files for the Pinoccio Mega 256rfr2 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for Pinoccio Mega 256rfr2 board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief As the CPU is too slow to handle 115200 baud, we set the default
*        baudrate to 9600 for this board
*/
/**
 * CPU is fast enough to send 115200 baud but receiving is not working
 */
#define UART_STDIO_BAUDRATE (9600U)

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED_PORT			PORTB
#define LED_PORT_DDR		DDRB

#define LED_BLUE_PIN		GPIO_PIN(1, 4)
#define LED_RED_PIN			GPIO_PIN(1, 5)
#define LED_GREEN_PIN		GPIO_PIN(1, 6)

#define BLUE			(1 << DDB4)
#define RED				(1 << DDB5)
#define GREEN			(1 << DDB6)

// TODO clean up
// kept for compatibility
#define LED0_PIN            GPIO_PIN(1, 7)
#define LED0_MASK           (1 << DDB7)
#define LED0_ON             (PORTB |=  LED0_MASK)
#define LED0_OFF            (PORTB &= ~LED0_MASK)
#define LED0_TOGGLE         (PORTB ^=  LED0_MASK)
/** @} */

/**
 * @name Define the interface to the AT86RF231 radio
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAMS_BOARD      {.spi = 0, \
                                     .spi_speed = 86F2, \
                                     .cs_pin = 0, \
                                     .int_pin = 0, \
                                     .sleep_pin = 0, \
                                     .reset_pin = 0}

/**
 * Context swap defines
 * Setup to use PB7 which is pin change interrupt 7 (PCINT7)
 * This is configured by Setting  "Pin Change Interrupt Enable 0"
 * and enable PCINT7 at "Pin Change Mask Register 0"
 * This emulates a software triggered interrupt
 *
 * Be aware of the alternate Functions OC0A/OC1C/PCINT7 of Pin B7
 * and that this can't be used.
 *
 * Output Compare and PWM Output A for Timer/Counter0
 * Output Compare and PWM Output C for Timer/Counter1
 * or Pin Change Interrupt 7
 * Its D2 on the Pinoccio board.
 **/
 /*////////////////////////////////////////////////////////////
 *	ON port E0 is RX USB UART do NOT use as Context swap
 */////////////////////////////////////////////////////////////
// TODO Check if the not implemented I/O pin interrupt could be used to not waste pin,
// use PCINT4 until then.
// Note that the I/O ports corresponding to PCINT23:16 are not implemented. Therefore PCIE2 has no function in this device
#define AVR_CONTEXT_SWAP_INIT do { \
    DDRB |= (1 << PB0); \
    PCICR |= (1 << PCIE0); \
    PCMSK0 |= (1 << PCINT0); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  PCINT0_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTB ^= (1 << PB0)



/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
