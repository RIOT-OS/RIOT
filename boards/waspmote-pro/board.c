/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 INRIA, Francisco Acosta
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
 * @brief       Board specific implementations for the Waspmote PRO v1.2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <avr/io.h>

#include "board.h"
#include "cpu.h"
#include "riot_stdio.h"

void led_init(void);
void SystemInit(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
    /* initialize UART_1 on AUX1 */
    SET_MUX_AUX1_MODULE;

#ifdef XBEE_UART
#if XBEE_UART == 0
    /* initialize UART0 on SOCKET0 (XBee) */
    SET_MUX_SOCKET0;
#else
    /* Initialize UART0 on USB */
    SET_MUX_USB_MODULE;
#endif
#endif

    /* initialize stdio via UART_STDIO_DEV */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();

    irq_enable();
}


/**
 * @brief Initialize the boards on-board LEDs (green and red)
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED_GREEN: PC1
 * - LED_RED: PD6
 */
void led_init(void)
{
    LED0_ENABLE_PORT;
    LED_GREEN_ON;
    LED1_ENABLE_PORT;
    LED_RED_ON;
}

/**
 * @brief Initialize the System, initialize IO via UART_0
 */
void SystemInit(void)
{
    /* initialize UART_0 for use as stdout */
    stdio_init();

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    puts("\f");
}

static int uart_putchar(char c, FILE *stream)
{
    (void) stream;
    stdio_write(&c, 1);
    return 0;
}

int uart_getchar(FILE *stream)
{
    (void) stream;
    char c;
    stdio_read(&c, 1);
    return (int)c;
}
