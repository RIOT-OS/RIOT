/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

static uint64_t _long_division(uint64_t n, uint64_t d);

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);
    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ);
            UART_0_DEV.INTENSET.bit.RXC = 1;
        break;
    }
    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    /* Calculate the BAUD value */
    uint64_t temp1 = ((16 * ((uint64_t)baudrate)) << 32);
    uint64_t ratio = _long_division(temp1 , UART_0_REF_F);
    uint64_t scale = ((uint64_t)1 << 32) - ratio;
    uint64_t baud_calculated = (65536 * scale) >> 32;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* Enable the peripheral channel */
            GCLK->PCHCTRL[SERCOM3_GCLK_ID_CORE].reg |= GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;

            while (!(GCLK->PCHCTRL[SERCOM3_GCLK_ID_CORE].reg & GCLK_PCHCTRL_CHEN)) {
                /* Wait for clock synchronization */
            }

            MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM3;
            /* configure PINS to input/output*/
            UART_0_PORT.DIRSET.reg = (1 << UART_0_TX_PIN);  /* tx's direction is output */
            UART_0_PORT.PINCFG[UART_0_RX_PIN % 32].bit.INEN = true; /* buffer rx pin's value */

            /* enable PMUX for pins and set to config C. */
            UART_0_PORT.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG \
                                        | PORT_WRCONFIG_WRPMUX \
                                        | PORT_WRCONFIG_PMUX(0x2) \
                                        | PORT_WRCONFIG_PMUXEN \
                                        | UART_0_PINS;

            UART_0_DEV.CTRLA.bit.ENABLE = 0; /* Disable to write, need to sync tho */
            while(UART_0_DEV.SYNCBUSY.bit.ENABLE) {}

            /* set to LSB, asynchronous mode without parity, PAD0 Tx, PAD1 Rx,
             * 16x over-sampling, internal clk */
            UART_0_DEV.CTRLA.reg = SERCOM_USART_CTRLA_DORD \
                                    | SERCOM_USART_CTRLA_FORM(0x0) \
                                    | SERCOM_USART_CTRLA_SAMPA(0x0) \
                                    | SERCOM_USART_CTRLA_TXPO(0x0) \
                                    | SERCOM_USART_CTRLA_RXPO(0x1) \
                                    | SERCOM_USART_CTRLA_SAMPR(0x0) \
                                    | SERCOM_USART_CTRLA_MODE(0x1) \
                                    | (UART_0_RUNSTDBY ? SERCOM_USART_CTRLA_RUNSTDBY : 0);

            /* Set baud rate */
            UART_0_DEV.BAUD.bit.BAUD = baud_calculated;

            /* enable receiver and transmitter, one stop bit*/
            UART_0_DEV.CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
            while(UART_0_DEV.SYNCBUSY.bit.CTRLB) {}

            break;
#endif
        default:
            (void)baud_calculated;
            return -1;
    }

    uart_poweron(uart);
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if (uart == UART_0) {
        for (size_t i = 0; i < len; i++) {
            while (UART_0_DEV.INTFLAG.bit.DRE == 0) {}
            while(UART_0_DEV.SYNCBUSY.bit.ENABLE) {}
            UART_0_DEV.DATA.reg = data[i];
            while (UART_0_DEV.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {}
        }
    }
}

static inline void irq_handler(uint8_t uartnum, SercomUsart *dev)
{
    if (dev->INTFLAG.bit.RXC) {
        /* cleared by reading DATA regiser */
        uint8_t data = (uint8_t)dev->DATA.reg;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    else if (dev->INTFLAG.bit.ERROR) {
        /* clear error flag */
        dev->INTFLAG.bit.ERROR = 1;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void uart_poweron(uart_t uart)
{
    while (UART_0_DEV.SYNCBUSY.reg) {}
    UART_0_DEV.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_poweroff(uart_t uart)
{
    while (UART_0_DEV.SYNCBUSY.reg) {}
    UART_0_DEV.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
}

#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, &UART_0_DEV);
}
#endif

static uint64_t _long_division(uint64_t n, uint64_t d)
{
    int32_t i;
    uint64_t q = 0, r = 0, bit_shift;
    for (i = 63; i >= 0; i--) {
        bit_shift = (uint64_t)1 << i;

        r = r << 1;

        if (n & bit_shift) {
            r |= 0x01;
        }

        if (r >= d) {
            r = r - d;
            q |= bit_shift;
        }
    }

    return q;
}
