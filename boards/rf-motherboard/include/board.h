/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_rf-motherboard-l1 RF Motherboard L1
 * @ingroup     boards
 * @brief       Board specific files for the rf-motherboard-l1 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the rf-motherboard-l1 board.
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Define the UART to be used as stdio and its baudrate
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the hardware timer
 * @{
 */
#define HW_TIMER            TIMER_0
/** @} */

/**
 * @name Define the interface to the AT86RF231 radio
 * @{
 */
#define AT86RF212B_SPI       SPI_1
#define AT86RF212B_CS        GPIO(PORT_C,8)  // IO1
#define AT86RF212B_INT       GPIO(PORT_C,10) // IO3
#define AT86RF212B_RESET     GPIO(PORT_C,7)  // IO0
#define AT86RF212B_SLEEP     GPIO(PORT_C,9)  // IO2
#define AT86RF212B_SPI_CLK   SPI_SPEED_5MHZ
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_PORT      (GPIOC)
#define LED_GREEN_PIN       (0)

#define LED_ORANGE_PORT     (GPIOC)
#define LED_ORANGE_PIN      (1)

#define LED_RED_PORT        (GPIOD)
#define LED_RED_PIN         (2)

#define LED_BLUE_PORT       (GPIOC)
#define LED_BLUE_PIN        (4)

#define LED_YELLOW_PORT     (GPIOC)
#define LED_YELLOW_PIN      (5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_RED_PORT->ODR &= ~(1<<LED_RED_PIN))
#define LED_RED_OFF         (LED_RED_PORT->ODR |= (1<<LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_RED_PORT->ODR ^= (1<<LED_RED_PIN))

#define LED_GREEN_ON        (LED_GREEN_PORT->ODR &= ~(1<<LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_GREEN_PORT->ODR |= (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1<<LED_GREEN_PIN))

#define LED_ORANGE_ON       (LED_ORANGE_PORT->ODR &= ~(1<<LED_ORANGE_PIN))
#define LED_ORANGE_OFF      (LED_ORANGE_PORT->ODR |= (1<<LED_ORANGE_PIN))
#define LED_ORANGE_TOGGLE   (LED_ORANGE_PORT->ODR ^= (1<<LED_ORANGE_PIN))

#define LED_BLUE_ON         (LED_BLUE_PORT->ODR &= ~(1<<LED_BLUE_PIN))
#define LED_BLUE_OFF        (LED_BLUE_PORT->ODR |= (1<<LED_BLUE_PIN))
#define LED_BLUE_TOGGLE     (LED_BLUE_PORT->ODR ^= (1<<LED_BLUE_PIN))

#define LED_YELLOW_ON       (LED_YELLOW_PORT->ODR &= ~(1<<LED_YELLOW_PIN))
#define LED_YELLOW_OFF      (LED_YELLOW_PORT->ODR |= (1<<LED_YELLOW_PIN))
#define LED_YELLOW_TOGGLE   (LED_YELLOW_PORT->ODR ^= (1<<LED_YELLOW_PIN))

#define LED_ON(led)         ((led == 2 ? LED_RED_PORT : LED_GREEN_PORT)->ODR &= ~(1 << (led >= 3 ? led+1 : led)))
#define LED_OFF(led)        ((led == 2 ? LED_RED_PORT : LED_GREEN_PORT)->ODR |= (1 << (led >= 3 ? led+1 : led)))
#define LED_TOGGLE(led)     ((led == 2 ? LED_RED_PORT : LED_GREEN_PORT)->ODR ^= (1 << (led >= 3 ? led+1 : led)))
/** @} */

#if 0
/**
 * @name Make defines for the LM75A
 * @{
 */
#define LM75A_1_I2C_INTERFACE		I2C_0
#define LM75A_1_I2C_BAUDRATE		I2C_SPEED_NORMAL
#define LM75A_1_I2C_ADDRESS			0x48
#endif

#if 0
/**
 * @name Make defines for the LIS3DH
 * @{
 */
#define LIS3DH_SPI_INTERFACE		SPI_0
#define LIS3DH_SPI_CS				GPIO_14
#define LIS3DH_SPI_INT1				GPIO_10
#define LIS3DH_SPI_INT2				GPIO_11
#define LIS3DH_SPI_SCALE			LIS3DH_SCALE_2G
#endif

#if 0
/**
 * @name Make defines for the SPIRIT1
 * @{
 */
#define SPIRIT1_SPI_INTERFACE		SPI_1
#define SPIRIT1_INTFACE_SD			GPIO_15
#define SPIRIT1_SPI_CS				GPIO_13
#define SPIRIT1_SPI_GPIO0			GPIO_6
#define SPIRIT1_SPI_GPIO1			GPIO_7
#define SPIRIT1_SPI_GPIO2			GPIO_8
#define SPIRIT1_SPI_GPIO3			GPIO_9
#endif

#if 0
/**
 * @name Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;
#endif

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
/** @} */
