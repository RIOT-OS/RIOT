/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hermann Lelong <hermann@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "assert.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#define RXENABLE            (USART_CR1_RE | USART_CR1_RXNEIE)

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static inline USART_TypeDef *dev(uart_t uart)
{
    return uart_config[uart].dev;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    uint16_t mantissa;
    uint8_t fraction;
    uint32_t clk;

    assert(uart < UART_NUMOF);

    /* save ISR context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    /* configure TX pin */
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    /* set TX pin high to avoid garbage during further initialization */
    gpio_set(uart_config[uart].tx_pin);
#ifdef CPU_FAM_STM32F1
    gpio_init_af(uart_config[uart].tx_pin, GPIO_AF_OUT_PP);
#else
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].tx_af);
#endif
    /* configure RX pin */
    if (rx_cb) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN);
#ifndef CPU_FAM_STM32F1
        gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].rx_af);
#endif
    }
#ifdef MODULE_STM32_PERIPH_UART_HW_FC
    if (uart_config[uart].cts_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].cts_pin, GPIO_IN);
        gpio_init(uart_config[uart].rts_pin, GPIO_OUT);
#ifdef CPU_FAM_STM32F1
        gpio_init_af(uart_config[uart].rts_pin, GPIO_AF_OUT_PP);
#else
        gpio_init_af(uart_config[uart].cts_pin, uart_config[uart].cts_af);
        gpio_init_af(uart_config[uart].rts_pin, uart_config[uart].rts_af);
#endif
    }
#endif

    /* enable the clock */
    uart_poweron(uart);

    /* reset UART configuration -> defaults to 8N1 mode */
    dev(uart)->CR1 = 0;
    dev(uart)->CR2 = 0;
    dev(uart)->CR3 = 0;

    /* calculate and apply baudrate */
    clk = periph_apb_clk(uart_config[uart].bus) / baudrate;
    mantissa = (uint16_t)(clk / 16);
    fraction = (uint8_t)(clk - (mantissa * 16));
    dev(uart)->BRR = ((mantissa & 0x0fff) << 4) | (fraction & 0x0f);

    /* enable RX interrupt if applicable */
    if (rx_cb) {
        NVIC_EnableIRQ(uart_config[uart].irqn);
        dev(uart)->CR1 = (USART_CR1_UE | USART_CR1_TE | RXENABLE);
    }
    else {
        dev(uart)->CR1 = (USART_CR1_UE | USART_CR1_TE);
    }

#ifdef MODULE_STM32_PERIPH_UART_HW_FC
    if (uart_config[uart].cts_pin != GPIO_UNDEF) {
        /* configure hardware flow control */
        dev(uart)->CR3 = (USART_CR3_RTSE | USART_CR3_CTSE);
    }
#endif

    return UART_OK;
}

static inline void send_byte(uart_t uart, uint8_t byte)
{
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) \
    || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) \
    || defined(CPU_FAM_STM32F7)
    while (!(dev(uart)->ISR & USART_ISR_TXE)) {}
    dev(uart)->TDR = byte;
#else
    while (!(dev(uart)->SR & USART_SR_TXE)) {}
    dev(uart)->DR = byte;
#endif
}

static inline void wait_for_tx_complete(uart_t uart)
{
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) \
    || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) \
    || defined(CPU_FAM_STM32F7)
    while (!(dev(uart)->ISR & USART_ISR_TC)) {}
#else
    while (!(dev(uart)->SR & USART_SR_TC)) {}
#endif
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

#ifdef MODULE_PERIPH_DMA
    if (!len) {
        return;
    }
    if (uart_config[uart].dma != DMA_STREAM_UNDEF) {
        if (irq_is_in()) {
            uint16_t todo = 0;
            if (dev(uart)->CR3 & USART_CR3_DMAT) {
                /* DMA transfer for UART on-going */
                todo = dma_suspend(uart_config[uart].dma);
            }
            if (todo) {
                dma_stop(uart_config[uart].dma);
                dev(uart)->CR3 &= ~USART_CR3_DMAT;
            }
            for (unsigned i = 0; i < len; i++) {
                send_byte(uart, data[i]);
            }
            if (todo > 0) {
                wait_for_tx_complete(uart);
                dev(uart)->CR3 |= USART_CR3_DMAT;
                dma_resume(uart_config[uart].dma, todo);
            }
        }
        else {
            dma_acquire(uart_config[uart].dma);
            dev(uart)->CR3 |= USART_CR3_DMAT;
            dma_transfer(uart_config[uart].dma, uart_config[uart].dma_chan, data,
                         (void *)&dev(uart)->DR, len, DMA_MEM_TO_PERIPH, DMA_INC_SRC_ADDR);
            dma_release(uart_config[uart].dma);

            /* make sure the function is synchronous by waiting for the transfer to
             * finish */
            wait_for_tx_complete(uart);
            dev(uart)->CR3 &= ~USART_CR3_DMAT;
        }
        return;
    }
#endif
    for (size_t i = 0; i < len; i++) {
        send_byte(uart, data[i]);
    }
    /* make sure the function is synchronous by waiting for the transfer to
     * finish */
    wait_for_tx_complete(uart);
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);
#ifdef STM32_PM_STOP
    if (isr_ctx[uart].rx_cb) {
        pm_block(STM32_PM_STOP);
    }
#endif
    periph_clk_en(uart_config[uart].bus, uart_config[uart].rcc_mask);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    periph_clk_dis(uart_config[uart].bus, uart_config[uart].rcc_mask);
#ifdef STM32_PM_STOP
    if (isr_ctx[uart].rx_cb) {
        pm_unblock(STM32_PM_STOP);
    }
#endif
}

static inline void irq_handler(uart_t uart)
{
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) \
    || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) \
    || defined(CPU_FAM_STM32F7)

    uint32_t status = dev(uart)->ISR;

    if (status & USART_ISR_RXNE) {
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg, (uint8_t)dev(uart)->RDR);
    }
    if (status & USART_ISR_ORE) {
        dev(uart)->ICR |= USART_ICR_ORECF;    /* simply clear flag on overrun */
    }

#else

    uint32_t status = dev(uart)->SR;

    if (status & USART_SR_RXNE) {
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg, (uint8_t)dev(uart)->DR);
    }
    if (status & USART_SR_ORE) {
        /* ORE is cleared by reading SR and DR sequentially */
        dev(uart)->DR;
    }

#endif

    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(UART_DEV(0));
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(UART_DEV(1));
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(UART_DEV(2));
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(UART_DEV(3));
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(UART_DEV(4));
}
#endif

#ifdef UART_5_ISR
void UART_5_ISR(void)
{
    irq_handler(UART_DEV(5));
}
#endif
