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
 * CPU is fast enough to send 115200 baud
 * but receiving is not working, irspipe and fetching bytes seems to be to slow
 * too much data lost when using with terminal
 */
 #define UART_STDIO_BAUDRATE (9600U)

// #define UART_STDIO_BAUDRATE (19200)

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
//#define LED0_PIN            GPIO_PIN(1, 7)
//#define LED0_MASK           (1 << DDB7)
//#define LED0_ON             (PORTB |=  LED0_MASK)
//#define LED0_OFF            (PORTB &= ~LED0_MASK)
//#define LED0_TOGGLE         (PORTB ^=  LED0_MASK)
/** @} */


#define SSN PORTB0
#define SCK PORTB1
#define MOSI PORTB2
#define MISO PORTB3
#define D2 PORTB0


#define SCL PORTD0
#define SDA PORTD1
#define RX1 PORTD2
#define TX1 PORTD3
#define BKPKVCC_EN PORTD4
#define D8 PORTD5
#define BKPK_BUS PORTD6
#define CHGSTATUS PORTD7

#define RX0 PORTE0
#define TX0 PORTE1
#define D6 PORTE2
#define D3 PORTE3
#define D4 PORTE4
#define D5 PORTE5
#define D7 PORTE6
#define BATTALRT PORTE7

#define A0 PORTF0
#define A1 PORTF1
#define A2 PORTF2
#define A3 PORTF3
#define A4 PORTF4
#define A5 PORTF5
#define A6 PORTF6
#define A7 PORTF7


/**
 * @name Define the interface to the AT86RFR2 radio
 *
 *	No SPI and no external pins are used. its an SOC
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 *
 * But we have to define a AT86RF2XX_PARAMS_BOARD to overwrite
 * the default parameters in
 */
#define AT86RF2XX_PARAMS_BOARD      {}

/**
 * Context swap defines
 * Setup to use PB0 which is pin change interrupt 0 (PCINT7)
 * This is configured by Setting  "Pin Change Interrupt Enable 0"
 * and enable PCINT0 at "Pin Change Mask Register 0"
 * This emulates a software triggered interrupt
 *
 * Be aware of the alternate Functions
 * SS/PCINT0 (SPI Slave Select input or Pin Change Interrupt 0)
 * and that this can't be used.
 * Its SSN on the Pinoccio board.
 *
 *
 * PB7 =  OC0A/OC1C/PCINT
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
// Set Port as output
// Enable Pin Change Interrupt 0
// Enable Pin change Interrupt 7 in Mask

//#define AVR_CONTEXT_SWAP_INIT do {
//    DDRB |= (1 << PB0);
//    PCICR |= (1 << PCIE0);
//    PCMSK0 |= (1 << PCINT0);
//} while (0)
//#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  PCINT0_vect
//#define AVR_CONTEXT_SWAP_TRIGGER   PORTB ^= (1 << PB0)


/*
* Context Swap Interrupt with Port E7
*
* clear global interrupt flag
* Configure PE7 as output
* Disable INT7 in External Interrupt Mask Register to avoid triggering when setting options
* Set External Interrupt Control Register B to trigger INT7 at any edge
* Enable INT7 in External Interrupt Mask Register
*/
#define AVR_CONTEXT_SWAP_INIT do { \
	cli(); \
	DDRE  |= (1 << PE6);  \
	EIMSK &= ~(1<<INT6);  \
	EICRB |= (1<<ISC60);  \
	EICRB &= ~(1<<ISC61); \
	EIMSK |= (1<<INT6);   \
} while (0)

#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  INT6_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTE ^= (1 << PE6)

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
