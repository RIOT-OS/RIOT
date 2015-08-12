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
#include "periph/uart.h"

#ifndef MODULE_UART0
#include "ringbuffer.h"
#include "mutex.h"
#endif

#ifdef MODULE_UART0
#include "board_uart0.h"
#endif

/**
 * @brief use mutex for waiting on incoming UART chars
 */
#ifndef MODULE_UART0
static mutex_t uart_rx_mutex;
static char rx_buf_mem[STDIO_RX_BUFSIZE];
static ringbuffer_t rx_buf;
#endif

void led_init(void);
void SystemInit(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);


/**
 * @brief Receive a new character from the UART and put it into the receive
 * buffer
 *
 * @param[in] data    the newly received byte
 */

void rx_cb(void *arg, char data)
{
    LED_TOGGLE;
#ifndef MODULE_UART0
    ringbuffer_add_one(&rx_buf, data);
    mutex_unlock(&uart_rx_mutex);
#elif MODULE_UART0

    if (uart0_handler_pid) {
        uart0_handle_incoming(data);
        uart0_notify_thread();
    }

#endif
}

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
#ifndef MODULE_UART0
    mutex_init(&uart_rx_mutex);
    ringbuffer_init(&rx_buf, rx_buf_mem, STDIO_RX_BUFSIZE);
#endif
    uart_init(STDIO, STDIO_BAUDRATE, (uart_rx_cb_t) rx_cb, NULL, NULL);

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    puts("\f");
}

static int uart_putchar(char c, FILE *stream)
{
    uart_write_blocking(UART_0, c);
    return 0;
}

int uart_getchar(FILE *stream)
{
#ifndef MODULE_UART0

    if (rx_buf.avail == 0) {
        mutex_lock(&uart_rx_mutex);
    }

    return ringbuffer_get_one(&rx_buf);
#else
    LED_TOGGLE;
    char temp;
    temp = (char) uart0_readc();
    return temp;
#endif
}
