/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f3
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
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"

 #include "board.h"

/**
 * @brief Allocate memory to store the callback functions
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

int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    USART_TypeDef *dev = 0;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;

    /* check if given UART device is valid */
    if (uart < 0 || uart >= UART_NUMOF) {
        return -1;
    }

    /* get UART device address */
    dev = _dev(uart);
    /* remember the context */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].tx_cb = tx_cb;
    uart_ctx[uart].arg = arg;

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init(uart_config[uart].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].af);
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].af);
    /* power on device */
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
    /* configure UART to 8N1 mode and enable both RX and TX */
    dev->CR3 = 0;
    dev->CR2 = 0;
    dev->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    /* enable global and RX interrupts */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dev->CR1 |= USART_CR1_RXNEIE;
    LED_GREEN_ON;
    return 0;
}

void uart_tx(uart_t uart)
{
    _dev(uart)->CR1 |= USART_CR1_TXEIE;
}

void uart_write(uart_t uart, char data)
{
    _dev(uart)->TDR = (uint8_t)data;
}

void uart_write_blocking(uart_t uart, char data)
{
    while (!(_dev(uart)->ISR & USART_ISR_TXE));
    _dev(uart)->TDR = (uint8_t)data;
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

static inline void irq_handler(int uart)
{
    USART_TypeDef *dev = _dev(uart);
    LED_RED_ON;

    if (dev->ISR & USART_ISR_RXNE) {
        char data = (char)dev->RDR;
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, data);
    }
    else if (dev->ISR & USART_ISR_TXE) {
        uint16_t data = uart_ctx[uart].tx_cb(uart_ctx[uart].arg);
        if (data) {
            dev->TDR = (uint8_t)data;
        }
        else {
            dev->CR1 &= ~(USART_CR1_TXEIE);
        }
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

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(4);
}
#endif
