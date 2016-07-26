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

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

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
#if UART_1_EN
        case UART_1:
        /* configure and enable global device interrupts */
        NVIC_SetPriority(UART_1_IRQ, UART_IRQ_PRIO);
        NVIC_EnableIRQ(UART_1_IRQ);
        /* enable RX interrupt */
        UART_1_DEV->IER |= (1 << 0);
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

            /* power on UART device and select peripheral clock */
            UART_0_CLKEN();
            UART_0_CLKSEL();
            /* set mode to 8N1 and enable access to divisor latch */
            UART_0_DEV->LCR = ((0x3 << 0) | (1 << 7));
            /* set baud rate registers (fixed for now) */
            UART_0_DEV->DLM = 0;
            UART_0_DEV->DLL = 13;
            /* enable FIFOs */
            UART_0_DEV->FCR = 1;
            /* select and configure the pin for RX */
            UART_0_RX_PINSEL &= ~(0x3 << (UART_0_RX_PIN * 2));
            UART_0_RX_PINSEL |= (UART_0_AF << (UART_0_RX_PIN * 2));
            UART_0_RX_PINMODE &= ~(0x3 << (UART_0_RX_PIN * 2));
            UART_0_RX_PINMODE |= (0x2 << (UART_0_RX_PIN * 2));
            /* select and configure the pin for TX */
            UART_0_TX_PINSEL &= ~(0x3 << (UART_0_TX_PIN * 2));
            UART_0_TX_PINSEL |= (UART_0_AF << (UART_0_TX_PIN * 2));
            UART_0_TX_PINMODE &= ~(0x3 << (UART_0_TX_PIN * 2));
            UART_0_TX_PINMODE |= (0x2 << (UART_0_TX_PIN * 2));
            /* disable access to divisor latch */
            UART_0_DEV->LCR &= ~(1 << 7);
            break;
#endif
#if UART_1_EN
        case UART_1:
            /* this implementation only supports 115200 baud */
            if (baudrate != 115200) {
                return -2;
            }

            /* power on UART device and select peripheral clock */
            UART_1_CLKEN();
            UART_1_CLKSEL();
            /* set mode to 8N1 and enable access to divisor latch */
            UART_1_DEV->LCR = ((0x3 << 0) | (1 << 7));
            /* set baud rate registers (fixed for now) */
            UART_1_DEV->DLM = 0;
            UART_1_DEV->DLL = 13;
            /* enable FIFOs */
            UART_1_DEV->FCR = 1;
            /* select and configure the pin for RX */
            UART_1_RX_PINSEL &= ~(0x3 << (UART_1_RX_PIN * 2));
            UART_1_RX_PINSEL |= (UART_1_AF << (UART_1_RX_PIN * 2));
            UART_1_RX_PINMODE &= ~(0x3 << (UART_1_RX_PIN * 2));
            UART_1_RX_PINMODE |= (0x2 << (UART_1_RX_PIN * 2));
            /* select and configure the pin for TX */
            UART_1_TX_PINSEL &= ~(0x3 << (UART_1_TX_PIN * 2));
            UART_1_TX_PINSEL |= (UART_1_AF << (UART_1_TX_PIN * 2));
            UART_1_TX_PINMODE &= ~(0x3 << (UART_1_TX_PIN * 2));
            UART_1_TX_PINMODE |= (0x2 << (UART_1_TX_PIN * 2));
            /* disable access to divisor latch */
            UART_1_DEV->LCR &= ~(1 << 7);
            break;
#endif
        default:
            return -1;
    }

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    LPC_UART_TypeDef *dev;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = (LPC_UART_TypeDef *)UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = (LPC_UART_TypeDef *)UART_1_DEV;
            break;
#endif
        default:
            return;
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev->LSR & (1 << 5)));       /* wait for THRE bit to be set */
        dev->THR = data[i];
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
#if UART_1_EN
        case UART_1:
            UART_1_CLKEN();
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
#if UART_1_EN
        case UART_1:
            UART_1_CLKDIS();
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

#if UART_1_EN
void UART_1_ISR(void)
{
    if (UART_1_DEV->LSR & (1 << 0)) {       /* is RDR flag set? */
        uint8_t data = (uint8_t)UART_1_DEV->RBR;
        config[UART_1].rx_cb(config[UART_1].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif
