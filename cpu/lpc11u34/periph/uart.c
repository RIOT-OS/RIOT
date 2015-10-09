/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the LPC11U34
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard the file in case no UART is defined */
#if (UART_0_EN)

/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    uart_rx_cb_t rx_cb;         /**< receive callback */
    uart_tx_cb_t tx_cb;         /**< transmit callback */
    void *arg;                  /**< callback argument */
} uart_conf_t;

/**
 * @brief UART device configurations
 */
static uart_conf_t config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* configure and enable global device interrupts */
            NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ);
            /* enable RX interrupt */
            UART_0_DEV->IER |= (1 << 0);
            break;
#endif
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* this implementation only supports 115200 baud */
            if (baudrate != 115200) {
                return -2;
            }

            /* select and configure the pin for RX */
            UART_0_RX_PINSEL &= ~0x07;
            UART_0_RX_PINSEL |= (UART_0_AF);
            /* select and configure the pin for TX */
            UART_0_TX_PINSEL &= ~0x07;
            UART_0_TX_PINSEL |= (UART_0_AF);

            /* power on UART device and select peripheral clock */
            UART_0_CLKEN();
            UART_0_CLKSEL();
            /* set mode to 8N1 and enable access to divisor latch */
            UART_0_DEV->LCR = ((0x3 << 0) | (1 << 7)) | (3 << 4);
            /* set baud rate registers (fixed for now) */
            UART_0_DEV->DLM = 0;
            UART_0_DEV->DLL = 17;
            UART_0_DEV->FDR |= (8) | (15 << 4);
            /* disable access to divisor latch */
            UART_0_DEV->LCR &= ~0x80;
            /* enable FIFOs */
            UART_0_DEV->FCR = (1 << 0) | (1 << 1) | (1 << 2) | (2 << 6);
            break;
#endif
        default:
            return -1;
    }

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* enable TX interrupt */
            UART_0_DEV->IER |= (1 << 1);
            break;
#endif
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_DEV->THR = (uint8_t)data;;
            break;
#endif
        default:
            return -1;
    }

    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            while (!(UART_0_DEV->LSR & (1 << 0)));      /* wait for RDR bit to be set */

            *data = (char)UART_0_DEV->RBR;
            break;
#endif
        default:
            return -1;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            while (!(UART_0_DEV->LSR & (1 << 5)));       /* wait for THRE bit to be set */

            UART_0_DEV->THR = (uint8_t)data;
            break;
#endif
        default:
            return -1;
    }

    return 1;
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKEN();
            break;
#endif
    }
}

void uart_poweroff(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_CLKDIS();
            break;
#endif
    }
}

#if UART_0_EN
void UART_0_ISR(void)
{
    if (UART_0_DEV->LSR & (1 << 0)) {       /* is RDR flag set? */
        char data = (char)UART_0_DEV->RBR;
        config[UART_0].rx_cb(config[UART_0].arg, data);
    }

    if (UART_0_DEV->LSR & (1 << 5)) {       /* THRE flag set? */
        if (UART_0_DEV->IER & (1 << 1)) {
            if (config[UART_0].tx_cb(config[UART_0].arg) == 0) {
                /* disable TX interrupt */
                UART_0_DEV->IER &= ~(1 << 1);
            }
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif


#endif /* (UART_0_EN) */
