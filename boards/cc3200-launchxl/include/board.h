/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_cc3200 CC32000 launchpad
 * @ingroup     boards
 * @brief       Support for the Texas Instruments CC3200 launchpad board.
 * @{
 *
 * @file
 *
 * @author      Attilio Dona'
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>
#include "cpu.h"
#include "nwp_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * for requesting an IRQ when PIN value changes
 */
#define CHANGE 4

/**
 * PIN value high to low IRQ trigger
 */
#define FALLING 3

/**
 * PIN value low to high IRQ trigger
 */
#define RISING 2

/**
 * PIN value high IRQ trigger
 */
#define HIGH 1

/**
 * PIN value low IRQ trigger
 */
#define LOW  0

/**
 * red led id
 */
#define RED_LED 29

/**
 * green led id
 */
#define GREEN_LED 10

/**
 * yellow led id
 */
#define YELLOW_LED 9

/**
 * SW2 button id
 */
#define PUSH1 3

/**
 * SW3 button id
 */
#define PUSH2 11

#define GDO0_PIN 19
#define GDO2_PIN 2

/*
 * debug pin
 *
 *    gpio_init(DEBUG_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
 *
 */
#define DEBUG_PIN 5 // P1.5 on launchpad

#define ALARM_PIN 8

/*
 * RF settings
 */
#define FREQ868_BR1200_DEV5200
//#define A110LR09_ETSI_M7_GFSK_38_KBAUD

#if 0
/**
 * @name Define the interface to the CC110X radio
 * @{
 */
#define CC110X_SPI       SPI_0
#define CC110X_CS        18
#define CC110X_MOSI      15
#define CC110X_MISO      14
#define CC110X_CLK       7
#define CC110X_INT       GDO0_PIN
#define CC110X_RESET     1
#define CC110X_SLEEP     1
#define CC110X_SPI_CLK   SPI_SPEED_100KHZ
/** @} */
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               80000000

#define SEC_TO_TICKS(sec)   80000000*sec /**< Convert seconds to  clock ticks */
#define MSEC_TO_TICKS(msec) 80000*msec /**< Convert millisecs to  clock ticks */
#define USEC_TO_TICKS(usec) 80*usec /**< Convert microseconds to  clock ticks */


/**
 * hardware timers modules
 */
#define TIMER_NUM_CHANNELS 4

/**
 * timer module allocated for xtimer
 */
#define XTIMER  TIMER_0

/**
 * xtimer fixed channel value
 */
#define XTIMER_CHAN (0)


#if TIMER_0_MAX_VALUE == 0xffffff
#define XTIMER_MASK 0xff000000
#elif TIMER_0_MAX_VALUE == 0xffff
#define XTIMER_MASK 0xffff0000
#endif

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      115200
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */

#define RED_LED_ON          gpio_set(RED_LED)
#define RED_LED_OFF         gpio_clear(RED_LED)
#define RED_LED_TOGGLE      gpio_toggle(RED_LED)

#define YELLOW_LED_ON       gpio_set(YELLOW_LED)
#define YELLOW_LED_OFF      gpio_clear(YELLOW_LED)
#define YELLOW_LED_TOGGLE   gpio_toggle(YELLOW_LED)

#define GREEN_LED_ON        gpio_set(GREEN_LED)
#define GREEN_LED_OFF       gpio_clear(GREEN_LED)
#define GREEN_LED_TOGGLE    gpio_toggle(GREEN_LED)

/* Default to red if the color is not specified: */
#define LED_ON              RED_LED_ON
#define LED_OFF             RED_LED_OFF
#define LED_TOGGLE          RED_LED_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
/** @} */
