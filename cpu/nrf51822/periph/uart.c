/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "board.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    void (*rx_cb)(char);
    void (*tx_cb)(void);
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 *
 * TODO: this function needs to be implemented
 */
int uart_init(uart_t uart, uint32_t baudrate, void (*rx_cb)(char), void (*tx_cb)(void))
{

    int res;

    /* initialize UART in blocking mode fist */
    res = uart_init_blocking(uart, baudrate);
    if (res != 0) {
        return res;
    }

    return -1;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    uint32_t baudrate_real;

    switch (baudrate) {
        case 1200:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud1200;
            break;
        case 2400:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud2400;
            break;
        case 4800:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud4800;
            break;
        case 9600:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud9600;
            break;
        case 14400:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud14400;
            break;
        case 19200:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud19200;
            break;
        case 28800:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud28800;
            break;
        case 38400:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud38400;
            break;
        case 57600:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud57600;
            break;
        case 76800:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud76800;
            break;
        case 115200:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud115200;
            break;
        case 230400:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud230400;
            break;
        case 250000:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud250000;
            break;
        case 460800:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud460800;
            break;
        case 921600:
            baudrate_real = UART_BAUDRATE_BAUDRATE_Baud921600;
            break;
        default:
            return -1;
    }

    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* reset configuration registers */
            UART_0_DEV->CONFIG = 0;

            /* select baudrate */
            UART_0_DEV->BAUDRATE = baudrate_real;

            /* configure UART pins to use */
            UART_0_DEV->PSELTXD = UART_0_PIN_TX;
            UART_0_DEV->PSELRXD = UART_0_PIN_RX;

            /* enable hw-flow control if defined */
#if UART_0_HWFLOWCTRL
            UART_0_DEV->PSELRTS = UART_0_PIN_RTS;
            UART_0_DEV->PSELCTS = UART_0_PIN_CTS;
            UART_0_DEV->CONFIG |= 1;        /* enable HW flow control */
#else
            UART_0_DEV->PSELRTS = 0xffffffff;       /* pin disconnected */
            UART_0_DEV->PSELCTS = 0xffffffff;       /* pin disconnected */
#endif

            /* enable the UART device */
            UART_0_DEV->ENABLE = UART_ENABLE_ENABLE_Enabled;

            /* enable TX and RX */
            UART_0_DEV->TASKS_STARTTX = 1;
            UART_0_DEV->TASKS_STARTRX = 1;
            break;
#endif
        case UART_UNDEFINED:
            return -2;
            break;
    }

    DEBUG("UART INITIALIZATION complete\n");

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    /* TODO: to be implemented */
}

void uart_tx_end(uart_t uart)
{
    /* TODO: to be implemented */
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            UART_0_DEV->TXD = (uint8_t)data;
            break;
        case UART_UNDEFINED:
            return -1;
    }

    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
        case UART_0:
            DEBUG("READING CHAR\n");
            /* wait for until data was received (RXDRDY == 1) */
            while (UART_0_DEV->EVENTS_RXDRDY != 1);
            DEBUG("RXDRDY was set\n");
            /* reset RXDRDY flag */
            UART_0_DEV->EVENTS_RXDRDY = 0;
            /* read new byte from receive data register */
            DEBUG("Reading data\n");
            *data = (char)(UART_0_DEV->RXD & 0xff);
            break;
        case UART_UNDEFINED:
            return -1;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            /* write data into transmit register */
            UART_0_DEV->TXD = (uint8_t)data;
            /* wait for any transmission to be done */
            while (UART_0_DEV->EVENTS_TXDRDY == 0);
            /* reset ready flag */
            UART_0_DEV->EVENTS_TXDRDY = 0;
            break;
        case UART_UNDEFINED:
            return -1;
    }

    return 1;
}
