/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard the file in case no UART is defined */
#if UART_0_EN

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
    if (uart == UART_0) {
        int res = uart_init_blocking(uart, baudrate);
        if (res < 0) {
            return res;
        }
        /* save callbacks */
        config[UART_0].rx_cb = rx_cb;
        config[UART_0].tx_cb = tx_cb;
        config[UART_0].arg = arg;
        /* configure and enable global device interrupts */
        NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
        NVIC_EnableIRQ(UART_0_IRQ);
        /* enable RX interrupt */
        UART_0_DEV->IER |= (1 << 0);
        return 0;
    }
    return -1;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    if (uart == UART_0) {
        /* this implementation only supports 115200 baud */
        if (baudrate != 115200) {
            return -2;
        }
        /* power on UART device */
        UART_0_CLKEN();
        /* set peripheral clock to CCLK / 4 */
        LPC_SC->PCLKSEL1 &= (0x3 << 18);
        /* set mode to 8N1 and enable access to divisor latch */
        UART_0_DEV->LCR = ((0x3 << 0) | (1 << 7));
        /* set baud rate registers (fixed for now) */
        UART_0_DEV->DLM = 0;
        UART_0_DEV->DLL = 13;
        /* enable FIFOs */
        UART_0_DEV->FCR = 1;
        /* select and configure pins */
        UART_0_PINSEL &= ~((0x3 << (UART_0_RX_PIN * 2)) | (0x3 << (UART_0_TX_PIN * 2)));
        UART_0_PINSEL |= ((UART_0_AF << (UART_0_RX_PIN * 2)) | (UART_0_AF << (UART_0_TX_PIN * 2)));
        UART_0_PINMODE &= ~((0x3 << (UART_0_RX_PIN * 2)) | (0x3 << (UART_0_TX_PIN * 2)));
        UART_0_PINMODE |= ((0x2 << (UART_0_RX_PIN * 2)) | (0x2 << (UART_0_TX_PIN * 2)));
        /* disable access to divisor latch */
        UART_0_DEV->LCR &= ~(1 << 7);
        return 0;
    }
    return -1;
}

void uart_tx_begin(uart_t uart)
{
    if (uart == UART_0) {
        /* enable TX interrupt */
        UART_0_DEV->IER |= (1 << 1);
    }
}

int uart_write(uart_t uart, char data)
{
    if (uart == UART_0) {
        UART_0_DEV->THR = (uint8_t)data;
        return 1;
    }
    return -1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    if (uart == UART_0) {
        while (!(UART_0_DEV->LSR & (1 << 0)));      /* wait for RDR bit to be set */
        *data = (char)UART_0_DEV->RBR;
        return 1;
    }
    return -1;
}

int uart_write_blocking(uart_t uart, char data)
{
    if (uart == UART_0) {
        while (!(UART_0_DEV->LSR & (1 << 5)));       /* wait for THRE bit to be set */
        UART_0_DEV->THR = (uint8_t)data;
        return 1;
    }
    return -1;
}

void uart_poweron(uart_t uart)
{
    if (uart == UART_0) {
        UART_0_CLKEN();
    }
}

void uart_poweroff(uart_t uart)
{
    if (uart == UART_0) {
        UART_0_CLKDIS();
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

#endif /* UART_0_EN */
