/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph/uart.h"
#include "periph/gpio.h"

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/**
 * @brief   Get the base register for the given UART device
 */
static inline USART_TypeDef *_dev(uart_t uart)
{
    return uart_config[uart].dev;
}

/**
 * @brief   Find out which peripheral bus the UART device is connected to
 *
 * @return  1: APB1
 * @return  2: APB2
 */
static inline int _bus(uart_t uart)
{
    return (uart_config[uart].rcc_mask < RCC_APB1ENR_USART2EN) ? 2 : 1;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    USART_TypeDef *dev;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;

    /* check if given UART device does exist */
    if (uart < 0 || uart >= UART_NUMOF) {
        return -1;
    }

    /* get UART base address */
    dev = _dev(uart);
    /* remember callback addresses and argument */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init(uart_config[uart].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].af);
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].af);
    /* enable UART clock */
    uart_poweron(uart);
    /* calculate and set baudrate */
    if (_bus(uart) == 1) {
        divider = CLOCK_APB1 / (16 * baudrate);
    }
    else {
        divider = CLOCK_APB2 / (16 * baudrate);
    }
    mantissa = (uint16_t)divider;
    fraction = (uint8_t)((divider - mantissa) * 16);
    dev->BRR = ((mantissa & 0x0fff) << 4) | (0x0f & fraction);
    /* configure UART to 8N1 and enable receive and transmit mode */
    dev->CR3 = 0;
    dev->CR2 = 0;
    dev->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    /* enable global and receive interrupts */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dev->CR1 |= USART_CR1_RXNEIE;
    return 0;
}

void uart_write(uart_t uart, const char* data, size_t len)
{
    for (int i = 0; i < len; i++) {
        while (!(_dev(uart)->SR & USART_SR_TXE));
        _dev(uart)->DR = (uint8_t)data[i];
    }
}

void uart_poweron(uart_t uart)
{
    if (_bus(uart) == 1) {
        RCC->APB1ENR |= uart_config[uart].rcc_mask;
    }
    else {
        RCC->APB2ENR |= uart_config[uart].rcc_mask;
    }
}

void uart_poweroff(uart_t uart)
{
    if (_bus(uart) == 1) {
        RCC->APB1ENR &= ~(uart_config[uart].rcc_mask);
    }
    else {
        RCC->APB2ENR &= ~(uart_config[uart].rcc_mask);
    }
}

static inline void irq_handler(int uart, USART_TypeDef *dev)
{
    if (dev->SR & USART_SR_RXNE) {
        char data = (char)dev->DR;
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0, uart_config[0].dev);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1, uart_config[1].dev);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2, uart_config[2].dev);
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(3, uart_config[3].dev);
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(4, uart_config[4].dev);
}
#endif

#ifdef UART_5_ISR
void UART_5_ISR(void)
{
    irq_handler(5, uart_config[5].dev);
}
#endif
