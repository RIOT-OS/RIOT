/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t ctx[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    Uart *dev;

    /* make sure given device is valid */
    if (uart >= UART_NUMOF) {
        return -1;
    }

    /* get base register */
    dev = uart_config[uart].dev;

    /* register callback */
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    /* enable clock */
    uart_poweron(uart);

    /* configure pins
       TODO: optimize once GPIO refactoring is merged */
    uart_config[uart].rx_port->PIO_PDR = (1 << uart_config[uart].rx_pin);
    uart_config[uart].rx_port->PIO_ABSR &= ~(1 << uart_config[uart].rx_pin);
    uart_config[uart].tx_port->PIO_ABSR |=  (uart_config[uart].mux <<
                                             uart_config[uart].rx_pin);
    uart_config[uart].tx_port->PIO_PDR = (1 << uart_config[uart].tx_pin);
    uart_config[uart].tx_port->PIO_ABSR &= ~(1 << uart_config[uart].tx_pin);
    uart_config[uart].tx_port->PIO_ABSR |=  (uart_config[uart].mux <<
                                             uart_config[uart].tx_pin);

    /* configure baud rate and set mode to 8N1 */
    dev->UART_BRGR = (F_CPU / (16 * baudrate));
    dev->UART_MR = UART_MR_PAR_NO | US_MR_CHRL_8_BIT;
    dev->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;

    /* enable RX interrupt */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dev->UART_IER = UART_IER_RXRDY;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    Uart *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        while (!(dev->UART_SR & UART_SR_TXRDY));
        dev->UART_THR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    PMC->PMC_PCER0 |= (1 << uart_config[uart].pmc_id);
}

void uart_poweroff(uart_t uart)
{
    PMC->PMC_PCER0 &= ~(1 << uart_config[uart].pmc_id);
}

static inline void isr_handler(int num)
{
    Uart *dev = uart_config[num].dev;

    if (dev->UART_SR & UART_SR_RXRDY) {
        ctx[num].rx_cb(ctx[num].arg, (char)dev->UART_RHR);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    isr_handler(0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    isr_handler(1);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    isr_handler(2);
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    isr_handler(3);
}
#endif
