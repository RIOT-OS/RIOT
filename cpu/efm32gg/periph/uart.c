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

#include <math.h>

//#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/uart.h"

#include "em_device.h"
#include "em_usart.h"
#include "em_gpio.h"

#define USART_INT_FLAGS (USART_IF_TXC | USART_IF_RXDATAV)

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void* arg;
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


int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    int res;

    /* initialize UART in blocking mode first */
    res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* enable global interrupt and configure the interrupts priority */
    switch (uart) {
#if UART_0_EN
    case UART_0:
        //NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
        //Enable interrupt vectors
        NVIC_EnableIRQ(UART_0_RX_IRQ);
        NVIC_EnableIRQ(UART_0_TX_IRQ);
        //Enable rx interrupt

        break;
#endif
#if UART_1_EN
    case UART_1:
        //NVIC_SetPriority(UART_1_IRQ, UART_IRQ_PRIO);
        NVIC_EnableIRQ(UART_1_RX_IRQ);
        NVIC_EnableIRQ(UART_1_TX_IRQ);
        UART_1_DEV->CR1 |= USART_CR1_RXNEIE;
        break;
#endif
    default:
        return -2;
    }

    /* register callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    return 0;
}

//Blocking UART initialisation
int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    USART_TypeDef *dev;
    GPIO_Port_TypeDef port;
    uint32_t rx_pin, tx_pin;

    //Default configuration
    USART_InitAsync_TypeDef uart_init = USART_INITASYNC_DEFAULT;
    uart_init.baudrate = baudrate;

    /* enable UART and port clocks and select devices */
    switch (uart) {
#if UART_0_EN
    case UART_0:
        dev = UART_0_DEV;
        port = UART_0_PORT;
        rx_pin = UART_0_RX_PIN;
        tx_pin = UART_0_TX_PIN;
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
        /* enable clocks */
        UART_1_CLKEN();
        UART_1_PORT_CLKEN();
        break;
#endif
    default:
        return -2;
    }

    // Configure pins
    GPIO_PinModeSet(port, tx_pin, gpioModePushPull, 1);
    GPIO_PinModeSet(port, rx_pin, gpioModeInput, 0);

    // Configure UART
    USART_InitAsync(dev, &uart_init);

    // Set route
    dev->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | UART_0_ROUTE;

    // Enable TX and RX
    USART_Enable(dev, usartEnable);

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
    case UART_0:
        USART_IntEnable(UART_0_DEV, USART_INT_FLAGS);
        break;
#endif
#if UART_1_EN
    case UART_1:
        USART_IntEnable(UART_1_DEV, USART_INT_FLAGS);
        break;
#endif
    default:
        break;
    }
}

void uart_tx_end(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
    case UART_0:
        USART_IntDisable(UART_0_DEV, USART_INT_FLAGS);
        break;
#endif
#if UART_1_EN
    case UART_1:
        USART_IntDisable(UART_1_DEV, USART_INT_FLAGS);
        break;
#endif
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
    default:
        return -1;
    }

    USART_Tx(dev, data);

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
    default:
        return -1;
    }

    // nb. emlib implementation is blocking
    *data = (char)USART_Rx(dev);

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
    default:
        return -1;
    }

    // nb. emlib implementation is blocking
    USART_Tx(dev, (uint8_t)data);

    return 1;
}

#ifdef UART_0_EN
__attribute__((naked)) void UART_0_RX_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_0, UART_0_DEV);
    ISR_EXIT();
}

__attribute__((naked)) void UART_0_TX_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_0, UART_0_DEV);
    ISR_EXIT();
}
#endif

#ifdef UART_1_EN
__attribute__((naked)) void UART_1_RX_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_1, UART_1_DEV);
    ISR_EXIT();
}

__attribute__((naked)) void UART_1_TX_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_1, UART_1_DEV);
    ISR_EXIT();
}
#endif

static inline void irq_handler(uint8_t uartnum, USART_TypeDef *dev)
{
    if (dev->IF & USART_IF_RXDATAV) {
        char data = (char)USART_RxDataGet(dev);
        config[uartnum].rx_cb(data, config[uartnum].arg);

    } else if (dev->IF & USART_IF_TXC) {
        config[uartnum].tx_cb(config[uartnum].arg);
        dev->IFC |= USART_IFC_TXC;
    }
}
