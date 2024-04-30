/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
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
* @brief   As the CPU is too slow to handle 115200 baud, we set the default
*          baudrate to 9600 for this board
*/
#define STDIO_UART_BAUDRATE  (9600U)

/**
 * @brief   Use the UART 0 for STDIO on this board, if the XBee socket is not
 *          being used
 */
#ifdef XBEE_UART
#if XBEE_UART == 0
#define STDIO_UART_DEV       (UART_DEV(1))
#else
#define STDIO_UART_DEV       (UART_DEV(0))
#endif
#endif

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PORT            PORTD
#define LED1_PORT            PORTC
#define LED0_PIN             (1 << 6)
#define LED1_PIN             (1 << 1)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs.
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
 * @name    Usage of LED to turn on when a kernel panic occurs.
 * @{
 */
#define LED_PANIC            LED_RED_ON
/** @} */

/**
 * @name    Macros for controlling the on-board MUXes.
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
 * @name CPU clock scale for waspmote-pro
 * @{
 */
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
/** @} */

/**
 * @name    xtimer configuration values
 * @warning This configuration is not actually compatible with xtimer. Sadly,
 *          no compatible clock frequency can be generated with the given core
 *          frequency
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_HZ                   (230400LU)
#define XTIMER_BACKOFF              (80)
#define XTIMER_ISR_BACKOFF          (120)
/** @} */

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE         ZTIMER_TYPE_PERIPH_TIMER    /**< Use periph_timer for ZTIMER_USEC */
#define CONFIG_ZTIMER_USEC_DEV          (TIMER_DEV(0))              /**< Use TIMER_DEV(0) for ZTIMER_USEC */
#define CONFIG_ZTIMER_USEC_BASE_FREQ    (230400LU)                  /**< Run timer for ZTIMER_USEC at 230,400 Hz */
#define CONFIG_ZTIMER_USEC_WIDTH        (16)                        /**< TIMER_DEV(0) is 16 bit wide */
/** @} */

/**
 * @name Onboard micro-sd slot pin definitions
 * @{
 */
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(0)  /**< SPI device */
#define SDCARD_SPI_PARAM_CS         SD_SS       /**< Chip Select */
#define SDCARD_SPI_PARAM_CLK        SD_SCK      /**< Serial Clock */
#define SDCARD_SPI_PARAM_MOSI       SD_MOSI     /**< Master Output, Slave Input */
#define SDCARD_SPI_PARAM_MISO       SD_MISO     /**< Master Input, Slave Output */
#define SDCARD_SPI_PARAM_POWER      MEM_PW      /**< Powen on/off */
#define SDCARD_SPI_PARAM_POWER_AH   (true)      /**< Power on with power pin high */
#define CARD_DETECT_PIN             SD_PRESENT  /**< Pin for card detect */

/** @} */

/** Default MTD device */
#define MTD_0 mtd_dev_get(0)

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
