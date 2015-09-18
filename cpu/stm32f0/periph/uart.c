/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
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
#include "periph_conf.h"
#include "periph/uart.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

static USART_TypeDef *const uart_port[UART_NUMOF] = {
#if UART_0_EN
    [UART_0] = UART_0_DEV,
#endif
#if UART_1_EN
    [UART_1] = UART_1_DEV,
#endif
};


int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    USART_TypeDef *dev = 0;
    GPIO_TypeDef *port = 0;
    uint32_t rx_pin = 0;
    uint32_t tx_pin = 0;
    uint8_t af = 0;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;

    /* enable UART and port clocks and select devices */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            port = UART_0_PORT;
            rx_pin = UART_0_RX_PIN;
            tx_pin = UART_0_TX_PIN;
            af = UART_0_AF;
            /* enable clocks */
            UART_0_CLKEN();
            UART_0_PORT_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            port = UART_1_PORT;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            af = UART_1_AF;
            /* enable clocks */
            UART_1_CLKEN();
            UART_1_PORT_CLKEN();
            break;
#endif
    }

    /* configure RX and TX pins, set pin to use alternative function mode */
    port->MODER &= ~(3 << (rx_pin * 2) | 3 << (tx_pin * 2));
    port->MODER |= 2 << (rx_pin * 2) | 2 << (tx_pin * 2);
    /* and assign alternative function */
    if (rx_pin < 8) {
        port->AFR[0] &= ~(0xf << (rx_pin * 4));
        port->AFR[0] |= af << (rx_pin * 4);
    }
    else {
        port->AFR[1] &= ~(0xf << ((rx_pin - 16) * 4));
        port->AFR[1] |= af << ((rx_pin - 16) * 4);
    }
    if (tx_pin < 8) {
        port->AFR[0] &= ~(0xf << (tx_pin * 4));
        port->AFR[0] |= af << (tx_pin * 4);
    }
    else {
        port->AFR[1] &= ~(0xf << ((tx_pin - 16) * 4));
        port->AFR[1] |= af << ((tx_pin - 16) * 4);
    }

    /* configure UART to mode 8N1 with given baudrate */
    divider = ((float)F_CPU) / (16 * baudrate);
    mantissa = (uint16_t)divider;
    fraction = (uint8_t)((divider - mantissa) * 16);
    dev->BRR = ((mantissa & 0x0fff) << 4) | (0x0f & fraction);

    /* enable receive and transmit mode */
    dev->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

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
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;

    return 0;
}

void uart_tx(uart_t dev)
{
    USART_TypeDef *uart = uart_port[dev];
    uart->CR1 |= USART_CR1_TXEIE;
}

void uart_write_blocking(uart_t dev, char data)
{
    USART_TypeDef *uart = uart_port[dev];

    while (!(uart->ISR & USART_ISR_TXE));
    uart->TDR = (uint8_t)data;
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

static inline void irq_handler(uart_t uartnum, USART_TypeDef *dev)
{
    if (dev->ISR & USART_ISR_RXNE) {
        char data = (char)dev->RDR;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    else if (dev->ISR & USART_ISR_ORE) {
        /* do nothing on overrun */
        dev->ICR |= USART_ICR_ORECF;
    }
    else if (dev->ISR & USART_ISR_TXE) {
        uint16_t data = uart_config[uartnum].tx_cb(uart_config[uartnum].arg);
        if (data) {
            dev->TDR = (uint8_t)data;
        }
        else {
            dev->CR1 &= ~USART_CR1_TXEIE;
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(UART_0, UART_0_DEV);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(UART_1, UART_1_DEV);
}
#endif
