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
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "mutex.h"
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
 * @brief   Transmission locks
 */
static mutex_t _tx_dma_sync[UART_NUMOF];
static mutex_t _tx_lock[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    USART_TypeDef *dev;
    DMA_Channel_TypeDef *channel;
    uint32_t bus_clk;
    uint16_t mantissa;
    uint8_t fraction;

    /* check if given UART device does exist */
    if (uart >= UART_NUMOF) {
        return -1;
    }

    /* get UART base address */
    dev = _dev(uart);
    /* remember callback addresses and argument */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;
    /* init TX lock and DMA synchronization mutex */
    mutex_init(&_tx_lock[uart]);
    mutex_init(&_tx_dma_sync[uart]);
    mutex_lock(&_tx_dma_sync[uart]);

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    gpio_init_af(uart_config[uart].tx_pin, GPIO_AF_OUT_PP);

    /* enable UART clock */
    uart_poweron(uart);

    /* calculate and apply baudrate */
    bus_clk = (uart_config[uart].bus == APB1) ? CLOCK_APB1 : CLOCK_APB2;
    bus_clk /= baudrate;
    mantissa = (uint16_t)(bus_clk / 16);
    fraction = (uint8_t)(bus_clk - (mantissa * 16));
    dev->BRR = ((mantissa & 0x0fff) << 4) | (fraction & 0x0f);

    /* configure UART to 8N1 and enable receive and transmit mode */
    dev->CR3 = USART_CR3_DMAT;
    dev->CR2 = 0;
    dev->CR1 = (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE
                | USART_CR1_RXNEIE);

    /* configure the DMA channel for transmission */
    dma_poweron(uart_config[uart].dma_chan);
    channel = dma_channel(uart_config[uart].dma_chan);
    channel->CCR = DMA_CCR_PL_0 | DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
    channel->CPAR = (uint32_t)&(dev->DR);

    /* enable the UART's global interrupt and activate it */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dma_isr_enable(uart_config[uart].dma_chan);

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    /* in case we are inside an ISR, we need to send blocking */
    if (inISR()) {
        /* send data by active waiting on the TXE flag */
        USART_TypeDef *dev = _dev(uart);
        for (int i = 0; i < len; i++) {
            while (!(dev->SR & USART_SR_TXE)) {}
            dev->DR = data[i];
        }
    }
    else {
        mutex_lock(&_tx_lock[uart]);
        DMA_Channel_TypeDef *channel = dma_channel(uart_config[uart].dma_chan);
        /* configure and start DMA transfer */
        channel->CMAR = (uint32_t)data;
        channel->CNDTR = (uint16_t)len;
        channel->CCR |= DMA_CCR_EN;
        /* wait for transfer to complete */
        mutex_lock(&_tx_dma_sync[uart]);
        mutex_unlock(&_tx_lock[uart]);
    }
}

void uart_poweron(uart_t uart)
{
    periph_clk_en(uart_config[uart].bus, uart_config[uart].rcc_mask);
}

void uart_poweroff(uart_t uart)
{
    periph_clk_dis(uart_config[uart].bus, uart_config[uart].rcc_mask);
}

static inline void irq_handler(int uart, USART_TypeDef *dev)
{
    uint32_t status = _dev(uart)->SR;

    if (status & USART_SR_RXNE) {
        char data = (char)_dev(uart)->DR;
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, data);
    }
    if (status & USART_SR_ORE) {
        /* ORE is cleared by reading SR and DR sequentially */
        _dev(uart)->DR;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

static inline void dma_handler(int uart, int channel)
{
    /* clear DMA done flag */
    dma_base(channel)->IFCR = dma_tcif(channel);
    mutex_unlock(&_tx_dma_sync[uart]);
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0, uart_config[0].dev);
}

void UART_0_DMA_ISR(void)
{
    dma_handler(0, uart_config[0].dma_chan);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1, uart_config[1].dev);
}

void UART_1_DMA_ISR(void)
{
    dma_handler(1, uart_config[1].dma_chan);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2, uart_config[2].dev);
}

void UART_2_DMA_ISR(void)
{
    dma_handler(2, uart_config[2].dma_chan);
}
#endif
