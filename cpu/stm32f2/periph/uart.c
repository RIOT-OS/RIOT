/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hermann Lelong <hermann@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Pieter Willemsen <pieter.willemsen@altran.com>
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "periph/dma.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static inline void uart_dma_enable(USART_TypeDef *uart_dev);
static inline void uart_dma_disable(USART_TypeDef *uart_dev);
void static inline write_byte(uart_t uart, uint8_t value);

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
 */
static inline bus_t _bus(uart_t uart)
{
    return (uart_config[uart].rcc_mask < RCC_APB1ENR_USART2EN) ? APB2 : APB1;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    USART_TypeDef *dev;
    float divider;
    uint16_t mantissa;
    uint8_t fraction;
    uint32_t max_clock;
    uint8_t over8;

    /* check if given UART device does exist */
    if (uart < 0 || uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* check if baudrate is reachable and choose the right oversampling method*/
    max_clock = (_bus(uart) == APB1) ? CLOCK_APB1 : CLOCK_APB2;

    if (baudrate < (max_clock / 16)) {
        over8 = 0;
    }
    else if (baudrate < (max_clock / 8)) {
        over8 = 1;
    }
    else {
        return UART_NOBAUD;
    }

    /* get UART base address */
    dev = _dev(uart);
    /* remember callback addresses and argument */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, uart_config[uart].rx_mode);
    gpio_init(uart_config[uart].tx_pin, uart_config[uart].tx_mode);
    gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].af);
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].af);
    /* enable UART clock */
    uart_poweron(uart);

    /* calculate and set baudrate */
    divider = max_clock / (8 * (2 - over8) * baudrate);

    mantissa = (uint16_t)divider;
    fraction = (uint8_t)((divider - mantissa) * (8 * (2 - over8)));
    dev->BRR = ((mantissa & 0x0fff) << 4) | (0x07 & fraction);
    /* configure UART to 8N1 and enable receive and transmit mode*/
    dev->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    if (over8) {
        dev->CR1 |= USART_CR1_OVER8;
    }
    dev->CR3 = 0;
    dev->CR2 = 0;

    if (uart_config[uart].hw_flow_ctrl) {
        gpio_init(uart_config[uart].cts_pin, uart_config[uart].cts_mode);
        gpio_init(uart_config[uart].rts_pin, uart_config[uart].rts_mode);
        gpio_init_af(uart_config[uart].cts_pin, uart_config[uart].af);
        gpio_init_af(uart_config[uart].rts_pin, uart_config[uart].af);
        DEBUG("Init flow control on uart %u\n", uart);
        /* configure hardware flow control */
        dev->CR3 |= USART_CR3_RTSE | USART_CR3_CTSE;
    }

    /* dma init */
    dma_stream_init(uart_config[uart].dma_stream);

    /* enable global and receive interrupts */
    NVIC_EnableIRQ(uart_config[uart].irqn);

    dev->CR1 |= USART_CR1_RXNEIE;

    return UART_OK;
}

void static inline write_byte(uart_t uart, uint8_t value)
{
    USART_TypeDef *dev = _dev(uart);

    while (!(dev->SR & USART_SR_TXE)) {}
    dev->DR = value;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    /* in case we are inside an ISR, we need to send blocking */
    USART_TypeDef *dev = _dev(uart);

    if (irq_is_in()) {
        /* send data by active waiting on the TXE flag */
        uint16_t todo = dma_suspend(uart_config[uart].dma_stream);
        if (todo > 0) {
            uart_dma_disable(dev);
#ifdef DEVELHELP
            write_byte(uart, '<');
#endif
        }
#ifdef DEVELHELP
        write_byte(uart, '|');
#endif
        for (int i = 0; i < len; i++) {
            while (!(dev->SR & USART_SR_TXE)) {}
            dev->DR = data[i];
        }
        if (todo > 0) {
#ifdef DEVELHELP
            write_byte(uart, '>');
#endif
            while (!(dev->SR & USART_SR_TXE)) {}
            uart_dma_enable(dev);
            dma_resume(uart_config[uart].dma_stream, todo);
        }
    }
    else {
        uint32_t tx_conf = ((uart_config[uart].dma_chan << 25) |
                            DMA_SxCR_PL_0 |
                            DMA_SxCR_MINC |
                            DMA_SxCR_DIR_0 |
                            DMA_SxCR_TCIE);

        /* acquire dma uart stream */
        dma_conf_acquire(uart_config[uart].dma_stream);
        /* UART stream configuration */
        dma_stream_config(uart_config[uart].dma_stream, (uint32_t)&(dev->DR),
                          tx_conf, (char *) data, (uint16_t)len);

        uart_dma_enable(dev);
        /* dma run */
        dma_enable(uart_config[uart].dma_stream);
        /* wait until end of transmission */
        dma_transmission_acquire(uart_config[uart].dma_stream);

        uart_dma_disable(dev);
        /* disable dma */
        dma_disable(uart_config[uart].dma_stream);
        /* release dma uart stream */
        dma_conf_release(uart_config[uart].dma_stream);
    }
}

static inline void uart_dma_enable(USART_TypeDef *uart_dev)
{
    /* enable the selected uart DMA transmission requests */
    uart_dev->CR3 |= USART_CR3_DMAT;
}

static inline void uart_dma_disable(USART_TypeDef *uart_dev)
{
    /* disable the selected uart DMA transmission requests */
    uart_dev->CR3 &= (uint16_t) ~USART_CR3_DMAT;
}

void uart_poweron(uart_t uart)
{
    periph_clk_en(_bus(uart), uart_config[uart].rcc_mask);
}

void uart_poweroff(uart_t uart)
{
    periph_clk_dis(_bus(uart), uart_config[uart].rcc_mask);
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
