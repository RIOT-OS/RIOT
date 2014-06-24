/*
 * Copyright (C) 2014 Freie Universität Berlin
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


/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    void (*rx_cb)(char);
    void (*tx_cb)(void);
} uart_conf_t;

/**
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param uart          the UART device that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, USART_TypeDef *uart);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, void (*rx_cb)(char), void (*tx_cb)(void))
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;

    /* enable receive interrupt */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            UART_0_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            UART_1_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
#if UART_2_EN
        case UART_2:
            NVIC_SetPriority(UART_2_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_2_IRQ_CHAN);
            UART_2_DEV->CR1 |= USART_CR1_RXNEIE;
            break;
#endif
        case UART_UNDEFINED:
        default:
            return -2;
            break;
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    USART_TypeDef *dev;
    GPIO_TypeDef *port;
    uint32_t tx_pin;
    uint32_t rx_pin;
    uint8_t af;
    float clk;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            port = UART_0_PORT;
            clk = UART_0_CLK;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            af = UART_0_AF;
            UART_0_CLKEN();
            UART_0_PORT_CLKEN();
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            port = UART_1_PORT;
            clk = UART_1_CLK;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            af = UART_1_AF;
            UART_1_CLKEN();
            UART_1_PORT_CLKEN();
            break;
#endif
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            port = UART_2_PORT;
            clk = UART_2_CLK;
            tx_pin = UART_2_TX_PIN;
            rx_pin = UART_2_RX_PIN;
            af = UART_2_AF;
            UART_2_CLKEN();
            UART_2_PORT_CLKEN();
            break;
#endif
        case UART_UNDEFINED:
        default:
            return -1;
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
        port->AFR[1] &= ~(0xf << ((rx_pin - 8) * 4));
        port->AFR[1] |= af << ((rx_pin - 8) * 4);
    }
    if (tx_pin < 8) {
        port->AFR[0] &= ~(0xf << (tx_pin * 4));
        port->AFR[0] |= af << (tx_pin * 4);
    }
    else {
        port->AFR[1] &= ~(0xf << ((tx_pin - 8) * 4));
        port->AFR[1] |= af << ((tx_pin - 8) * 4);
    }

    /* configure UART to mode 8N1 with given baudrate */
    divider = clk / (16 * baudrate);
    mantissa = (uint16_t)divider;
    fraction = (uint8_t)((divider - mantissa) * 16);
    dev->BRR = ((mantissa & 0x0fff) << 4) | (0x0f & fraction);

    /* enable receive and transmit mode */
    dev->CR3 = 0;
    dev->CR2 = 0;
    dev->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

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
#if UART_2_EN
        case UART_2:
            UART_2_DEV->CR1 |= USART_CR1_TXEIE;
            break;
#endif
        case UART_UNDEFINED:
        default:
            break;
    }
}

void uart_tx_end(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            UART_0_DEV->CR1 &= ~USART_CR1_TXEIE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_DEV->CR1 &= ~USART_CR1_TXEIE;
            break;
#endif
#if UART_2_EN
        case UART_2:
            UART_2_DEV->CR1 &= ~USART_CR1_TXEIE;
            break;
#endif
        case UART_UNDEFINED:
        default:
            break;
    }
}

int uart_write(uart_t uart, char data)
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
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            break;
#endif
        case UART_UNDEFINED:
        default:
            return -2;
            break;
    }

    if (dev->ISR & USART_ISR_TXE) {
        dev->TDR = (uint8_t)data;
    }

    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
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
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            break;
#endif
        case UART_UNDEFINED:
        default:
            return -2;
            break;
    }

    while (!(dev->ISR & USART_ISR_RXNE));
    *data = (char)dev->RDR;

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
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
#if UART_2_EN
        case UART_2:
            dev = UART_2_DEV;
            break;
#endif
        case UART_UNDEFINED:
        default:
            return -2;
            break;
    }

    while (!(dev->ISR & USART_ISR_TXE));
    dev->TDR = (uint8_t)data;

    return 1;
}

#if UART_0_EN
__attribute__((naked)) void UART_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_0, UART_0_DEV);
    ISR_EXIT();
}
#endif

#if UART_1_EN
__attribute__((naked)) void UART_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_1, UART_1_DEV);
    ISR_EXIT();
}
#endif

#if UART_2_EN
__attribute__((naked)) void UART_2_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_2, UART_2_DEV);
    ISR_EXIT();
}
#endif

static inline void irq_handler(uint8_t uartnum, USART_TypeDef *dev)
{
    if (dev->ISR & USART_ISR_RXNE) {
        char data = (char)dev->RDR;
        config[uartnum].rx_cb(data);
    }
    else if (dev->ISR & USART_ISR_TXE) {
        config[uartnum].tx_cb();
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
