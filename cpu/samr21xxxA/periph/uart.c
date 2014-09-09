/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/uart.h"
#include "periph_conf.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];


int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    // /* initialize basic functionality */
    // int res = uart_init_blocking(uart, baudrate);
    // if (res != 0) {
    //     return res;
    // }
    //
    // /* register callbacks */
    // uart_config[uart].rx_cb = rx_cb;
    // uart_config[uart].tx_cb = tx_cb;
    // uart_config[uart].arg = arg;
    //
    // /* configure interrupts and enable RX interrupt */
    // switch (uart) {
    //     case UART_0:
    //         UART_0_DEV->UART_IER = UART_IER_RXRDY;
    //     break;
    // }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
//     uint16_t clock_divider = F_CPU / (16 * baudrate);
//     switch (uart) {
// #if UART_0_EN
//         case UART_0:
//             /* configure PINS */
//             UART_0_PORT->PIO_PDR = UART_0_PINS;
//             UART_0_PORT->PIO_ABSR &= ~UART_0_PINS;
//
//             /* set clock divider */
//             UART_0_DEV->UART_BRGR = clock_divider;
//             /* set to normal mode without parity */
//             UART_0_DEV->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;
//             /* enable receiver and transmitter and reset status bits */
//             UART_0_DEV->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;
//             break;
// #endif
//     }
    return 0;
}

void uart_tx_begin(uart_t uart)
{

}

void uart_tx_end(uart_t uart)
{

}

int uart_write(uart_t uart, char data)
{
    // switch (uart) {
    //     case UART_0:
    //         UART_0_DEV->UART_THR = data;
    //         break;
    // }
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    // switch (uart) {
    //     case UART_0:
    //         while (!(UART_0_DEV->UART_SR & UART_SR_RXRDY));
    //         *data = (char)UART_0_DEV->UART_RHR;
    //         break;
    // }
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    // switch (uart) {
    //     case UART_0:
    //         while (!(UART_0_DEV->UART_SR & UART_SR_TXRDY));
    //         UART_0_DEV->UART_THR = data;
    //         break;
    // }
    return 1;
}

void uart_poweron(uart_t uart)
{

}

void uart_poweroff(uart_t uart)
{

}

#endif /* UART_NUMOF */
