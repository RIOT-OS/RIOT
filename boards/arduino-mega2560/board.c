/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mega2560
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Arduino Mega 2560 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <avr/io.h>

#include "board.h"
#include "cpu.h"
#include "uart_stdio.h"

void led_init(void);
void SystemInit(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
    /* initialize stdio via USART_0 */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();

    enableIRQ();
}


/**
 * @brief Initialize the boards on-board LED (Amber LED "L")
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LED is connected to the following pin:
 * - LED: PB27
 */
void led_init(void)
{
    LED_ENABLE_PORT;
    LED_ON;
}

/**
 * @brief Initialize the System, initialize IO via UART_0
 */
void SystemInit(void)
{
    /* initialize UART_0 for use as stdout */
    uart_stdio_init();

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    puts("\f");
}

static int uart_putchar(char c, FILE *stream)
{
    uart_stdio_write(&c, 1);
    return 0;
}

int uart_getchar(FILE *stream)
{
    char c;
    uart_stdio_read(&c, 1);
    return (int)c;
}
