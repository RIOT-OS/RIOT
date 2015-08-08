/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_remote Re-Mote
 * @ingroup     boards
 * @brief       Support for the Re-Mote board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Re-Mote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (32000000UL)
/**
 * @name Assign the peripheral timer to be used as hardware timer
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
#define LED_PORT_C          GPIO_C
#define LED_PORT_D          GPIO_D
#define LED_RED_PIN         (2)
#define LED_GREEN_PIN       (5)
#define LED_BLUE_PIN        (3)
/** @} */
/**
 * @name Macros for controlling the on-board LEDs
 * @{
 */
#define LED_RED_ON         (LED_PORT_D->DATA &= ~(1 << LED_RED_PIN))
#define LED_RED_OFF        (LED_PORT_D->DATA |= (1 << LED_RED_PIN))
#define LED_RED_TOGGLE     (LED_PORT_D->DATA ^= (1 << LED_RED_PIN))
#define LED_GREEN_ON       (LED_PORT_D->DATA &= ~(1 << LED_GREEN_PIN))
#define LED_GREEN_OFF      (LED_PORT_D->DATA |= (1 << LED_GREEN_PIN))
#define LED_GREEN_TOGGLE   (LED_PORT_D->DATA ^= (1 << LED_GREEN_PIN))
#define LED_BLUE_ON        (LED_PORT_C->DATA &= ~(1 << LED_BLUE_PIN))
#define LED_BLUE_OFF       (LED_PORT_C->DATA |= (1 << LED_BLUE_PIN))
#define LED_BLUE_TOGGLE    (LED_PORT_C->DATA ^= (1 << LED_BLUE_PIN))
#define LED_ALL_OFF        LED_GREEN_OFF; \
                           LED_RED_OFF;   \
                           LED_BLUE_OFF
/* Output is color white */
#define LED_ALL_ON         LED_BLUE_ON;   \
                           LED_RED_ON;    \
                           LED_GREEN_ON
#define LED_YELLOW_ON      LED_BLUE_OFF;  \
                           LED_GREEN_ON;  \
                           LED_RED_ON
#define LED_YELLOW_OFF     LED_GREEN_OFF; \
                           LED_RED_OFF
#define LED_YELLOW_TOGGLE  LED_GREEN_TOGGLE; \
                           LED_RED_TOGGLE
#define LED_PURPLE_ON      LED_GREEN_OFF;    \
                           LED_BLUE_ON;      \
                           LED_RED_ON
#define LED_PURPLE_OFF     LED_BLUE_OFF;     \
                           LED_RED_OFF
#define LED_PURPLE_TOGGLE  LED_BLUE_TOGGLE;  \
                           LED_RED_TOGGLE
/* Take LED_COLOR as argument, i.e LED_RED */
#define LED_FADE(led)                         \
  volatile int i;                             \
  int k, j;                                   \
  LED_FADE_EXPAND(led)
#define LED_FADE_EXPAND(led)                  \
  for(k = 0; k < 800; ++k) {                  \
    j = k > 400 ? 800 - k : k;                \
    led##_ON;                                 \
    for(i = 0; i < j; ++i) {                  \
      asm("nop");                             \
    }                                         \
    led##_OFF;                                \
    for(i = 0; i < 400 - j; ++i) {            \
      asm("nop");                             \
    }                                         \
  }
#define LED_RAINBOW()                         \
  volatile int i;                             \
  int k,j;                                    \
  LED_FADE_EXPAND(LED_YELLOW);                \
  LED_FADE_EXPAND(LED_RED);                   \
  LED_FADE_EXPAND(LED_PURPLE);                \
  LED_FADE_EXPAND(LED_BLUE);                  \
  LED_FADE_EXPAND(LED_GREEN);                 \
/** @} */
/**
 * @name Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                1
#endif
#define CCA_BACKDOOR_ENABLE       1
#define CCA_BACKDOOR_PORT_A_PIN   3 /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL 0 /**< Active low */
/** @} */
/**
 * @name 2.4GHz RF switch controlled by SW
 * @{
 */
#define RF_SWITCH_PORT      GPIO_D
#define RF_SWITCH_PIN       (4)
#define RF_SWITCH_EXTERNAL  (RF_SWITCH_PORT->DATA |= (1 << RF_SWITCH_PIN))
#define RF_SWITCH_INTERNAL  (RF_SWITCH_PORT->DATA &= ~(1 << RF_SWITCH_PIN))
#define RF_SWITCH_TOGGLE    (RF_SWITCH_PORT->DATA ^= (1 << RF_SWITCH_PIN))
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
