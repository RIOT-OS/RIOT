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
#define LED_RED 29

/**
 * green led id
 */
#define LED_GREEN 10

/**
 * yellow led id
 */
#define LED_YELLOW 9

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

#define LED_RED_ON          gpio_set(LED_RED)
#define LED_RED_OFF         gpio_clear(LED_RED)
#define LED_RED_TOGGLE      gpio_toggle(LED_RED)

#define LED_YELLOW_ON       gpio_set(LED_YELLOW)
#define LED_YELLOW_OFF      gpio_clear(LED_YELLOW)
#define LED_YELLOW_TOGGLE   gpio_toggle(LED_YELLOW)

#define LED_GREEN_ON        gpio_set(LED_GREEN)
#define LED_GREEN_OFF       gpio_clear(LED_GREEN)
#define LED_GREEN_TOGGLE    gpio_toggle(LED_GREEN)

/* Default to red if the color is not specified: */
#define LED_ON              LED_RED_ON
#define LED_OFF             LED_RED_OFF
#define LED_TOGGLE          LED_RED_TOGGLE
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
