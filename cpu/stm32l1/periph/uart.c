/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  driver_periph
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
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph/gpio.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* do basic initialization */
    int res = init_base(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    /* enable receive interrupt */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            NVIC_EnableIRQ(UART_0_IRQ);
            UART_0_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_EnableIRQ(UART_1_IRQ);
            UART_1_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
#if UART_2_EN
        case UART_2:
            NVIC_EnableIRQ(UART_2_IRQ);
            UART_2_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
    }

    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    USART_TypeDef *dev = 0;
    gpio_t tx_pin = 0;
    gpio_t rx_pin = 0;
    gpio_af_t af = 0;
    float clk = 0;
    uint16_t mantissa;
    uint8_t fraction;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            clk = UART_0_CLK;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            af = UART_0_AF;
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            clk = UART_1_CLK;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            af = UART_1_AF;
            UART_1_CLKEN();
            break;
#endif
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            clk = UART_2_CLK;
            tx_pin = UART_2_TX_PIN;
            rx_pin = UART_2_RX_PIN;
            af = UART_2_AF;
            UART_2_CLKEN();
            break;
#endif
        default:
            return -1;
    }

    /* uart_configure RX and TX pins, set pin to use alternative function mode */
    gpio_init(tx_pin, GPIO_OUT);
    gpio_init_af(tx_pin, af);
    gpio_init(rx_pin, GPIO_IN);
    gpio_init_af(rx_pin, af);

    /* uart_configure UART to mode 8N1 with given baudrate */
    clk /= baudrate;
    mantissa = (uint16_t)(clk / 16);
    fraction = (uint8_t)(clk - (mantissa * 16));
    dev->BRR = ((mantissa & 0x0fff) << 4) | (0x0f & fraction);

    /* enable receive and transmit mode */
    dev->CR3 = 0;
    dev->CR2 = 0;
    dev->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    USART_TypeDef *dev = 0;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            break;
#endif
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            break;
#endif
        default:
            return;
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev->SR & USART_SR_TXE)) {}
        dev->DR = data[i];
    }
}

static inline void irq_handler(uint8_t uartnum, USART_TypeDef *dev)
{
    if (dev->SR & USART_SR_RXNE) {
        uint8_t data = (uint8_t)dev->DR;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, UART_0_DEV);
}
#endif

#if UART_1_EN
void UART_1_ISR(void)
{
    irq_handler(UART_1, UART_1_DEV);
}
#endif

#if UART_2_EN
void UART_2_ISR(void)
{
    irq_handler(UART_2, UART_2_DEV);
}
#endif
