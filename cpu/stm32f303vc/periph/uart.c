/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f303vg
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/uart.h"
#include "periph_conf.h"
#include "cpu.h"


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
    // vars
    NVIC_InitTypeDef nvic;

    // do the basic initialization
    int res = uart_init_blocking(uart, baudrate);
    if (res == 0) {
        return res;
    }

    // register callbacks
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;

    // configure interrupts
    nvic.NVIC_IRQChannelPreemptionPriority = UART_IRQ_PRIO;
    nvic.NVIC_IRQChannelSubPriority = 0x01;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    // port specific configuration
    switch (uart) {
        case UART_0:
            // setup irq
            nvic.NVIC_IRQChannel = UART_0_IRQ;
            NVIC_Init(&nvic);
            // enable receive interrupt
            USART_ITConfig(UART_0_DEV, USART_IT_RXNE, ENABLE);
            break;
        case UART_1:
            // setup irq
            nvic.NVIC_IRQChannel = UART_1_IRQ;
            NVIC_Init(&nvic);
            // enable receive interrupt
            USART_ITConfig(UART_1_DEV, USART_IT_RXNE, ENABLE);
            break;
        case UART_2:
            // setup irq
            nvic.NVIC_IRQChannel = UART_2_IRQ;
            NVIC_Init(&nvic);
            // enable receive interrupt
            USART_ITConfig(UART_2_DEV, USART_IT_RXNE, ENABLE);
            break;
        case UART_UNDEFINED:
        default:
            return -2;
    }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    // vars
    USART_InitTypeDef uart_init;
    GPIO_InitTypeDef gpio;

    // assing std values
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    uart_init.USART_BaudRate = baudrate;
    uart_init.USART_WordLength = USART_WordLength_8b;
    uart_init.USART_StopBits = USART_StopBits_1;
    uart_init.USART_Parity = USART_Parity_No;
    uart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    uart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    switch (uart) {
        case UART_0:
            // enable clocks
            UART_0_CLKEN();
            UART_0_PORT_CLKEN();
            // configure pins
            gpio.GPIO_Pin = UART_0_PINS;
            GPIO_Init(UART_0_PORT, &gpio);
            UART_0_RX_AFCFG();
            UART_0_TX_AFCFG();
            // setup UART devic
            USART_Init(UART_0_DEV, &uart_init);
            USART_Cmd(UART_0_DEV, ENABLE);
            break;
        case UART_1:
            // enable clocks
            UART_1_CLKEN();
            UART_1_PORT_CLKEN();
            // configure pins
            gpio.GPIO_Pin = UART_1_PINS;
            GPIO_Init(UART_1_PORT, &gpio);
            UART_1_RX_AFCFG();
            UART_1_TX_AFCFG();
            // setup UART device
            USART_Init(UART_1_DEV, &uart_init);
            USART_Cmd(UART_1_DEV, ENABLE);
            break;
        case UART_2:
            // enable clocks
            UART_2_CLKEN();
            UART_2_PORT_CLKEN();
            // configure pins
            gpio.GPIO_Pin = UART_2_PINS;
            GPIO_Init(UART_2_PORT, &gpio);
            UART_2_RX_AFCFG();
            UART_2_TX_AFCFG();
            // setup UART device
            USART_Init(UART_2_DEV, &uart_init);
            USART_Cmd(UART_2_DEV, ENABLE);
            break;
        case UART_UNDEFINED:
        default:
            return -2;
    }
    return 0;
}


void uart_tx_begin(uart_t uart)
{
    switch (uart) {
        case UART_0:
            USART_ITConfig(UART_0_DEV, USART_IT_TXE, ENABLE);
            break;
        case UART_1:
            USART_ITConfig(UART_1_DEV, USART_IT_TXE, ENABLE);
            break;
        case UART_2:
            USART_ITConfig(UART_2_DEV, USART_IT_TXE, ENABLE);
            break;
        case UART_UNDEFINED:
            break;
    }
}

void uart_tx_end(uart_t uart)
{
    switch (uart) {
        case UART_0:
            USART_ITConfig(UART_0_DEV, USART_IT_TXE, DISABLE);
            break;
        case UART_1:
            USART_ITConfig(UART_1_DEV, USART_IT_TXE, DISABLE);
            break;
        case UART_2:
            USART_ITConfig(UART_2_DEV, USART_IT_TXE, DISABLE);
            break;
        case UART_UNDEFINED:
            break;
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            if (USART_GetFlagStatus(UART_0_DEV, USART_FLAG_TXE) == SET) {
                USART_SendData(UART_0_DEV, (uint16_t)data);
                return 1;
            }
            break;
        case UART_1:
            if (USART_GetFlagStatus(UART_1_DEV, USART_FLAG_TXE) == SET) {
                USART_SendData(UART_1_DEV, (uint16_t)data);
                return 1;
            }
            break;
        case UART_2:
            if (USART_GetFlagStatus(UART_2_DEV, USART_FLAG_TXE) == SET) {
                USART_SendData(UART_2_DEV, (uint16_t)data);
                return 1;
            }
            break;
        case UART_UNDEFINED:
        default:
            return -1;
    }
    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
        case UART_0:
            while (USART_GetFlagStatus(UART_0_DEV, USART_FLAG_RXNE) == RESET);
            *data = (char)USART_ReceiveData(UART_0_DEV);
            break;
        case UART_1:
            while (USART_GetFlagStatus(UART_1_DEV, USART_FLAG_RXNE) == RESET);
            *data = (char)USART_ReceiveData(UART_1_DEV);
            break;
        case UART_2:
            while (USART_GetFlagStatus(UART_2_DEV, USART_FLAG_RXNE) == RESET);
            *data = (char)USART_ReceiveData(UART_2_DEV);
            break;
        case UART_UNDEFINED:
        default:
            return -1;
    }
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            while (USART_GetFlagStatus(UART_0_DEV, USART_FLAG_TXE) == RESET);
            USART_SendData(UART_0_DEV, (uint16_t)data);
            break;
        case UART_1:
            while (USART_GetFlagStatus(UART_1_DEV, USART_FLAG_TXE) == RESET);
            USART_SendData(UART_1_DEV, (uint16_t)data);
            break;
        case UART_2:
            while (USART_GetFlagStatus(UART_2_DEV, USART_FLAG_TXE) == RESET);
            USART_SendData(UART_2_DEV, (uint16_t)data);
            break;
        case UART_UNDEFINED:
        default:
            return -1;
    }
    return 1;
}


void UART_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_0, UART_0_DEV);
    ISR_EXIT();
}

void UART_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_1, UART_1_DEV);
    ISR_EXIT();
}

void UART_2_ISR(void)
{
    ISR_ENTER();
    irq_handler(UART_2, UART_2_DEV);
    ISR_EXIT();
}


/* --- local functions --------------------------------------------------------------------------*/
static inline void irq_handler(uint8_t uartnum, USART_TypeDef *uart)
{
    if (USART_GetITStatus(uart, USART_IT_RXNE) == SET) {        // a new byte arrived
        char data = (char)USART_ReceiveData(uart);
        config[uartnum].rx_cb(data);
    } else if (USART_GetITStatus(uart, USART_IT_TXE) == SET) {  // ready to send new byte
        config[uartnum].tx_cb();
    }
}
