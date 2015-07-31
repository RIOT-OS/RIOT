/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t config[UART_NUMOF];


int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    USART_TypeDef *dev;
    uint32_t bus_freq;
    gpio_t rx_pin, tx_pin;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;

    /* enable UART and port clocks and select devices */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            rx_pin = UART_0_RX_PIN;
            tx_pin = UART_0_TX_PIN;
            bus_freq = UART_0_BUS_FREQ;
            /* enable clocks */
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            bus_freq = UART_1_BUS_FREQ;
            /* enable clocks */
            UART_1_CLKEN();
            break;
#endif
        default:
            return -2;
    }
    /* configure RX and TX pin */
    gpio_init_af(tx_pin, GPIO_AF_OUT_PP);
    gpio_init(rx_pin, GPIO_DIR_IN, GPIO_NOPULL);

    /* configure UART to mode 8N1 with given baudrate */
    divider = ((float)bus_freq) / (16 * baudrate);
    mantissa = (uint16_t)floorf(divider);
    fraction = (uint8_t)floorf((divider - mantissa) * 16);
    dev->BRR = 0;
    dev->BRR |= ((mantissa & 0x0fff) << 4) | (0x0f & fraction);

    /* enable receive and transmit mode */
    dev->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    /* register callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    /* enable global interrupt and configure the interrupts priority */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ);
            UART_0_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_SetPriority(UART_1_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ);
            UART_1_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
        default:
            return -2;
    }
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_DEV->CR1 |= USART_CR1_TXEIE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_DEV->CR1 |= USART_CR1_TXEIE;
            break;
#endif
    }
}

void uart_write(uart_t uart, char data)
{
    USART_TypeDef *dev;

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
        default:
            return;
    }

    if (dev->SR & USART_SR_TXE) {
        dev->DR = (uint8_t)data;
    }
}

void uart_write_blocking(uart_t uart, char data)
{
    USART_TypeDef *dev;

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
        default:
            return;
    }

    while (!(dev->SR & USART_SR_TXE));
    dev->DR = (uint8_t)data;
}

static inline void irq_handler(uart_t uartnum, USART_TypeDef *dev)
{
    if (dev->SR & USART_SR_RXNE) {
        char data = (char)dev->DR;
        config[uartnum].rx_cb(config[uartnum].arg, data);
    }
    else if (dev->SR & USART_SR_ORE) {
        /* ORE is cleared by reading SR and DR sequentially */
        dev->DR;
    }
    else if (dev->SR & USART_SR_TXE) {
        if (config[uartnum].tx_cb(config[uartnum].arg) == 0) {
            dev->CR1 &= ~(USART_CR1_TXEIE);
        }
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
