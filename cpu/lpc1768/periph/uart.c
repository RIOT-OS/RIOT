/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

static inline void init_base(uart_t uart, uint32_t baudrate);

/**
 * @brief   Get the GPT register base for a timer
 *
 * @param[in] tim   index of the timer
 *
 * @return          base address
 */
static inline LPC_UART_TypeDef *dev(uart_t uart)
{
    assert(uart < UART_NUMOF);

    return ((LPC_UART_TypeDef *)uart_config[uart].dev);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);
    init_base(uart, baudrate);

    /* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    /* configure and enable global device interrupts */
    NVIC_SetPriority(uart_config[uart].irq_rx, UART_IRQ_PRIO);
    NVIC_EnableIRQ(uart_config[uart].irq_rx);
    /* enable RX interrupt */
    dev(uart)->IER |= (1 << 0);

    return UART_OK;
}

static inline void init_base(uart_t uart, uint32_t baudrate)
{
    /* Fixed baud rate. */
    assert(baudrate == 115200);
    assert(uart < UART_NUMOF);
    (void) baudrate;
    const uart_conf_t *cfg = &uart_config[uart];
    /* The RX/TX must be together */
    assert(cfg->pinsel_shift <= 27);
    /* power on UART device and select peripheral clock */
    LPC_SC->PCONP |= (1 << cfg->clk_offset);
    if (cfg->clk_offset >= 16) {
        LPC_SC->PCLKSEL1 &= ~((uint32_t)0x3 << ((cfg->clk_offset - 16) * 2));
    }
    else {
        LPC_SC->PCLKSEL0 &= ~((uint32_t)0x3 << (cfg->clk_offset * 2));
    }
    /* set mode to 8N1 and enable access to divisor latch */
    dev(uart)->LCR = ((0x3 << 0) | (1 << 7));
    /* set baud rate registers (fixed for now) */
    dev(uart)->DLM = 0;
    dev(uart)->DLL = 13;
    /* enable FIFOs */
    dev(uart)->FCR = 1;

    /* Clear register for mux selection */
    *(&LPC_PINCON->PINSEL0 + cfg->pinsel) &=
            ~((uint32_t)0xF << (cfg->pinsel_shift * 2));
    /* Select uart TX mux */
    *(&LPC_PINCON->PINSEL0 + cfg->pinsel) |=
            ((uint32_t)cfg->pinsel_af << (cfg->pinsel_shift * 2));
    /* Select uart RX mux */
    *(&LPC_PINCON->PINSEL0 + cfg->pinsel) |=
            ((uint32_t)cfg->pinsel_af << (cfg->pinsel_shift * 2 + 2));
    /* Clear modes for RX and TX pins */
    *(&LPC_PINCON->PINMODE0 + cfg->pinsel) &=
            ~((uint32_t)0xF << (cfg->pinsel_shift * 2));
    /* Set TX mode */
    *(&LPC_PINCON->PINMODE0 + cfg->pinsel) |=
            ((uint32_t)0x2 << (cfg->pinsel_shift * 2));
    /* Set RX mode */
    *(&LPC_PINCON->PINMODE0 + cfg->pinsel) |=
            ((uint32_t)0x2 << (cfg->pinsel_shift * 2 + 2));
    /* disable access to divisor latch */
    dev(uart)->LCR &= ~(1 << 7);
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);
    for (size_t i = 0; i < len; i++) {
        /* wait for THRE bit to be set */
        while (!(dev(uart)->LSR & (1 << 5))) {}
        dev(uart)->THR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);
    LPC_SC->PCONP |= (1 << uart_config[uart].clk_offset);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);
    LPC_SC->PCONP &= ~(1 << uart_config[uart].clk_offset);
}

static void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);
    if (dev(uart)->LSR & (1 << 0)) {
        uint8_t data = (uint8_t)dev(uart)->RBR;
        config[uart].rx_cb(config[uart].arg, data);
    }
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
