/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#include "sched.h"
#include "thread.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static inline USART_TypeDef *dev(uart_t uart)
{
    return uart_config[uart].dev;
}

static void clk_en(uart_t uart)
{
    if (uart_config[uart].bus == APB1) {
        RCC->APB1ENR |= uart_config[uart].rcc_pin;
    }
    else {
        RCC->APB2ENR |= uart_config[uart].rcc_pin;
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    uint32_t bus_clk;
    uint16_t mantissa;
    uint8_t fraction;

    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return -1;
    }

    /* save ISR context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    /* configure RX and TX pin */
    gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    gpio_init_af(uart_config[uart].tx_pin, GPIO_AF_OUT_PP);

    /* enable the clock */
    clk_en(uart);

    /* reset UART configuration -> defaults to 8N1 mode */
    dev(uart)->CR1 = 0;
    dev(uart)->CR2 = 0;
    dev(uart)->CR3 = 0;

    /* calculate and apply baudrate */
    bus_clk = (uart_config[uart].bus == APB1) ? CLOCK_APB1 : CLOCK_APB2;
    bus_clk /= baudrate;
    mantissa = (uint16_t)(bus_clk / 16);
    fraction = (uint8_t)(bus_clk - (mantissa * 16));
    dev(uart)->BRR = ((mantissa & 0x0fff) << 4) | (fraction & 0x0f);

    /* enable the UART's global interrupt and activate it */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dev(uart)->CR1 = (USART_CR1_UE | USART_CR1_TE |
                      USART_CR1_RE | USART_CR1_RXNEIE);

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while(!(dev(uart)->SR & USART_SR_TXE)) {}
        dev(uart)->DR = data[i];
    }
}

static inline void irq_handler(uart_t uart)
{
    uint32_t status = dev(uart)->SR;

    if (status & USART_SR_RXNE) {
        char data = (char)dev(uart)->DR;
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg, data);
    }
    if (status & USART_SR_ORE) {
        /* ORE is cleared by reading SR and DR sequentially */
        dev(uart)->DR;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2);
}
#endif
