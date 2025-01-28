/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_uart
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
#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
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
        return UART_NODEV;
    }

    /* get base register */
    dev = uart_config[uart].dev;

    /* register callback */
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    /* enable clock */
    uart_poweron(uart);

    /* reset configuration */
    dev->UART_CR = 0;
    dev->UART_IDR = 0x0000ffff;

    /* configure pins */
    gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);
    if (rx_cb) {
        gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    }

    /* configure baud rate and set mode to 8N1 */
    dev->UART_BRGR = (CLOCK_CORECLOCK / (16 * baudrate));
    dev->UART_MR = UART_MR_PAR_NO | US_MR_CHRL_8_BIT;

    if (rx_cb) {
        dev->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;
        NVIC_EnableIRQ(uart_config[uart].irqn);
        dev->UART_IER = UART_IER_RXRDY;
    }
    else {
        dev->UART_CR = UART_CR_TXEN | UART_CR_RSTSTA;
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    Uart *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        while (!(dev->UART_SR & UART_SR_TXRDY)) {}
        dev->UART_THR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    PMC->PMC_PCER0 = (1 << uart_config[uart].pmc_id);
}

void uart_poweroff(uart_t uart)
{
    PMC->PMC_PCDR0 = (1 << uart_config[uart].pmc_id);
}

static inline void isr_handler(int num)
{
    Uart *dev = uart_config[num].dev;

    if (dev->UART_SR & UART_SR_RXRDY) {
        ctx[num].rx_cb(ctx[num].arg, (uint8_t)dev->UART_RHR);
    }
    cortexm_isr_end();
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
