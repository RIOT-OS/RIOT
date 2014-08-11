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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "board.h"

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
 * @brief Data structure holding the callbacks and argument for each UART device
 */
static uart_conf_t uart_config;

/**
 * @brief Allocate memory to store the callback functions.
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{

    int res;

    /* initialize UART in blocking mode fist */
    res = uart_init_blocking(uart, baudrate);
    if (res != 0) {
        return res;
    }

    /* remember callback addresses and argument */
    uart_config.rx_cb = rx_cb;
    uart_config.tx_cb = tx_cb;
    uart_config.arg = arg;
    /* enable global and receiving interrupt */
    NVIC_SetPriority(UART0_IRQn, UART_IRQ_PRIO);
    NVIC_EnableIRQ(UART0_IRQn);
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    /* the NRF only supports 1 UART device, so we don't need a switch statement here */
    if (uart != UART_0) {
        return -1;
    }

    /* power on the UART device */
    NRF_UART0->POWER = 1;
    /* reset configuration registers */
    NRF_UART0->CONFIG = 0;
    /* configure RX/TX pin modes */
    NRF_GPIO->DIRSET = (1 << UART_PIN_TX);
    NRF_GPIO->DIRCLR = (1 << UART_PIN_RX);
    /* configure UART pins to use */
    NRF_UART0->PSELTXD = UART_PIN_TX;
    NRF_UART0->PSELRXD = UART_PIN_RX;
    /* enable HW-flow control if defined */
#if UART_HWFLOWCTRL
    /* set pin mode for RTS and CTS pins */
    NRF_GPIO->DIRSET = (1 << UART_PIN_RTS);
    NRF_GPIO->DIRSET = (1 << UART_PIN_CTS);
    /* configure RTS and CTS pins to use */
    NRF_UART0->PSELRTS = UART_PIN_RTS;
    NRF_UART0->PSELCTS = UART_PIN_CTS;
    NRF_UART0->CONFIG |= UART_CONFIG_HWFC_Msk;  /* enable HW flow control */
#else
    NRF_UART0->PSELRTS = 0xffffffff;            /* pin disconnected */
    NRF_UART0->PSELCTS = 0xffffffff;            /* pin disconnected */
#endif

    /* select baudrate */
    switch (baudrate) {
        case 1200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1200;
            break;
        case 2400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud2400;
            break;
        case 4800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud4800;
            break;
        case 9600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud9600;
            break;
        case 14400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud14400;
            break;
        case 19200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud19200;
            break;
        case 28800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud28800;
            break;
        case 38400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud38400;
            break;
        case 57600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud57600;
            break;
        case 76800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud76800;
            break;
        case 115200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
            break;
        case 230400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud230400;
            break;
        case 250000:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud250000;
            break;
        case 460800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud460800;
            break;
        case 921600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud921600;
            break;
        default:
            return -1;
    }

    /* enable the UART device */
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
    /* enable TX and RX */
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    if (uart == UART_0) {
        if (uart_config.tx_cb(uart_config.arg) != 0) {
            NRF_UART0->INTENSET = UART_INTENSET_TXDRDY_Msk;
        }
    }
}

int uart_write(uart_t uart, char data)
{
    if (uart == UART_0) {
        NRF_UART0->TXD = (uint8_t)data;
        NRF_UART0->EVENTS_TXDRDY = 0;
        return 1;
    }
    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    if (uart == UART_0) {
        /* wait for until data was received (RXDRDY == 1) */
        while (NRF_UART0->EVENTS_RXDRDY != 1);
        /* reset RXDRDY flag */
        NRF_UART0->EVENTS_RXDRDY = 0;
        /* read new byte from receive data register */
        *data = (char)(NRF_UART0->RXD & 0xff);
        return 1;
    }
    return 0;
}

int uart_write_blocking(uart_t uart, char data)
{
    if (uart == UART_0) {
        /* write data into transmit register */
        NRF_UART0->TXD = (uint8_t)data;
        /* wait for any transmission to be done */
        while (NRF_UART0->EVENTS_TXDRDY == 0);
        /* reset ready flag */
        NRF_UART0->EVENTS_TXDRDY = 0;
        return 1;
    }
    return 0;
}

void uart_poweron(uart_t uart)
{
    if (uart == UART_0) {
        NRF_UART0->POWER = 1;
    }
}

void uart_poweroff(uart_t uart)
{
    if (uart == UART_0) {
        NRF_UART0->POWER = 0;
    }
}

void isr_uart0(void)
{
    if (NRF_UART0->EVENTS_RXDRDY == 1) {
        NRF_UART0->EVENTS_RXDRDY = 0;
        char byte = (char)(NRF_UART0->RXD & 0xff);
        uart_config.rx_cb(uart_config.arg, byte);
    }
    if (NRF_UART0->EVENTS_TXDRDY == 1) {
        NRF_UART0->EVENTS_TXDRDY = 0;
        if (uart_config.tx_cb(uart_config.arg) == 0) {
            NRF_UART0->INTENCLR = UART_INTENSET_TXDRDY_Msk;
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* UART_NUMOF */
