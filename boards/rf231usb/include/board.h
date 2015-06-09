/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam3s
 * @{
 *
 * @file
 * @brief           CPU specific values
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef BOARD_H
#define BOARD_H

/* Board specifics for the atrf231usb and deRFusb 23E0x radio
 * sticks */
#ifndef __SAM3S4B__
  #define __SAM3S4B__
#endif

#include "sam3s.h"

#include "conf_clock.h"

/* "The RF231USB-RD is populated with an external 18.432MHz crystal
   oscillator, assembled on the top side of the printed circuit board
   close to the MCU. Connected to XIN/XOUT (pins 61 and 62) it delivers
   an accurate clock for the ATSAM3S4BA." */
#define F_CPU             (48000000)
#define HW_TIMER          TIMER_0

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/* LED Macros */
#define LED_PORT          (PIOA)
#define LED_RED_PIN       (PIO_PA19)
#define LED_ORANGE_PIN    (PIO_PA17)
#define LED_GREEN_PIN     (PIO_PA18)
#define LED_ALL_PINS      (LED_RED_PIN | LED_ORANGE_PIN | LED_GREEN_PIN)

#define LED_TOGGLE(PIN)   LED_PORT->PIO_ODSR ^= (PIN)
#define LED_ON(PIN)       LED_PORT->PIO_CODR  = (PIN)
#define LED_OFF(PIN)      LED_PORT->PIO_SODR  = (PIN)

#define LED_RED_ON        LED_ON(LED_RED_PIN)
#define LED_RED_OFF       LED_OFF(LED_RED_PIN)
#define LED_RED_TOGGLE    LED_TOGGLE(LED_RED_PIN)

#define LED_ORANGE_ON     LED_ON(LED_ORANGE_PIN)
#define LED_ORANGE_OFF    LED_OFF(LED_ORANGE_PIN)
#define LED_ORANGE_TOGGLE LED_TOGGLE(LED_ORANGE_PIN)

#define LED_GREEN_ON      LED_ON(LED_GREEN_PIN)
#define LED_GREEN_OFF     LED_OFF(LED_GREEN_PIN)
#define LED_GREEN_TOGGLE  LED_TOGGLE(LED_GREEN_PIN)

#define STDIO_RX_BUFSIZE    (64U)

/* @name NG_AT86RF233 configuration */
#define NG_AT86RF231_SPI       SPI_0
#define NG_AT86RF231_INT       GPIO(PA,1)       /* PA1  */
#define NG_AT86RF231_CS        GPIO(PA,11)      /* PA11 */
#define NG_AT86RF231_SLEEP     GPIO(PA,21)      /* PA21 */
#define NG_AT86RF231_RESET     GPIO(PA,24)      /* PA24 */
#define NG_AT86RF231_SPI_CLK   (SPI_SPEED_5MHZ)

#endif /* #ifndef BOARD_H */
