/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 * 
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * 
 * @}
 */

#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_usart.h"

#include "periph/uart.h"
#include "periph_conf.h"


/* --- local typedefs ---------------------------------------------------------------------------*/
typedef struct {
    void (*rx_cb)(char);
    void (*tx_cb)(void);
} uart_conf_t;


/* --- local function definitions ---------------------------------------------------------------*/
static inline void irq_handler(uart_t uartnum, USART_TypeDef *uart);


/* --- local variables --------------------------------------------------------------------------*/
static uart_conf_t config[UART_NUMOF];


/* --- public functions -------------------------------------------------------------------------*/
void uart_init(uart_t uart,
               uint32_t baudrate,
               void (*rx_cb)(char),
               void (*tx_cb)(void))
{
    // vars
    USART_InitTypeDef uart_init;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    // register callbacks
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;

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

    nvic.NVIC_IRQChannelPreemptionPriority = UART_IRQ_PRIO;
    nvic.NVIC_IRQChannelSubPriority = 0x01;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    // port specific configuration
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
            // setup irq
            nvic.NVIC_IRQChannel = UART_0_IRQ;
            NVIC_Init(&nvic);
            // enable receive interrupt
            USART_ITConfig(UART_0_DEV, USART_IT_RXNE, ENABLE);
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
            // setup irq
            nvic.NVIC_IRQChannel = UART_1_IRQ;
            NVIC_Init(&nvic);
            // enable receive interrupt
            USART_ITConfig(UART_1_DEV, USART_IT_RXNE, ENABLE);
        break;
    }
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
    }
}


int8_t uart_write(uart_t uart, char data)
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
        default:
            return -1;
        break;
    }
    return 0;
}


void UART_1_IRQ_HANDLER(void)
{
    irq_handler(UART_0, UART_0_DEV);
}

void UART_2_IRQ_HANDLER(void)
{
    irq_handler(UART_1, UART_1_DEV);
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