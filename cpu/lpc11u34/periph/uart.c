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

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

/**
 * @todo   Merge with uart_init()
 */
static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    int res = init_base(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* save callbacks */
    config[uart].rx_cb = rx_cb;
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

static int init_base(uart_t uart, uint32_t baudrate)
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

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if (uart == UART_0) {
        for (size_t i = 0; i < len; i++) {
            while (!(UART_0_DEV->LSR & (1 << 5)));
            UART_0_DEV->THR = data[i];
        }
    }
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
        uint8_t data = (uint8_t)UART_0_DEV->RBR;
        config[UART_0].rx_cb(config[UART_0].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif
