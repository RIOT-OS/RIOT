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
 * @ingroup     cpu_samr30
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Pierre Godicheau <pierre.godicheau@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/uart.h"
#include "periph/gpio.h"
/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

//static uint64_t _long_division(uint64_t n, uint64_t d);

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return UART_NODEV;
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
    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    /* set baudrate */
    uint32_t baud =  ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);

    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* Enable the peripheral channel */
            MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0;
            GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE].reg |= GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;

            while (!(GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE].reg & GCLK_PCHCTRL_CHEN)) {
                /* Wait for clock synchronization */
            }

            MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0;
            /* configure PINS to input/output*/
            gpio_init(GPIO_PIN(UART_0_PORT_NB,UART_0_RX_PIN), GPIO_IN);
            gpio_init_mux(GPIO_PIN(UART_0_PORT_NB,UART_0_RX_PIN),GPIO_MUX_D);
            gpio_init(GPIO_PIN(UART_0_PORT_NB,UART_0_TX_PIN), GPIO_OUT);
            gpio_init_mux(GPIO_PIN(UART_0_PORT_NB,UART_0_TX_PIN), GPIO_MUX_D);

            UART_0_DEV.CTRLA.bit.ENABLE = 0; /* Disable to write, need to sync tho */
            while(UART_0_DEV.SYNCBUSY.bit.ENABLE) {}

            /* set to LSB, asynchronous mode without parity, PAD0 Tx, PAD1 Rx,
             * 16x over-sampling, internal clk */
            UART_0_DEV.CTRLA.reg = SERCOM_USART_CTRLA_DORD \
                                    | SERCOM_USART_CTRLA_FORM(0x0) \
                                    | SERCOM_USART_CTRLA_SAMPA(0x0) \
                                    | SERCOM_USART_CTRLA_TXPO(0x2) \
                                    | SERCOM_USART_CTRLA_RXPO(0x1) \
                                    | SERCOM_USART_CTRLA_SAMPR(0x01) \
                                    | SERCOM_USART_CTRLA_MODE(0x1) \
                                    | (UART_0_RUNSTDBY ? SERCOM_USART_CTRLA_RUNSTDBY : 0);

            /* Set baud rate */
            UART_0_DEV.BAUD.FRAC.FP = (baud % 10);
            UART_0_DEV.BAUD.FRAC.BAUD = (baud / 10);

            /* enable receiver and transmitter, one stop bit*/
            UART_0_DEV.CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
            while(UART_0_DEV.SYNCBUSY.bit.CTRLB) {}

            break;
#endif
        default:
            (void)baud;
            return UART_NODEV;
    }

    uart_poweron(uart);
    return UART_OK;
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
        dev->INTFLAG.reg |= SERCOM_USART_INTFLAG_ERROR;
    }
    cortexm_isr_end();
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
