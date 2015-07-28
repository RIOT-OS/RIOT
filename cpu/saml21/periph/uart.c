/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "periph/uart.h"
#include "periph/gpio.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] uart      UART device identifier
 *
 * @return              base register address
 */
static inline SercomUsart *_uart(uart_t uart)
{
    return uart_config[uart].dev;
}

int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    uint32_t baud;
    SercomUsart *dev;

    /* check if device is valid */
    if (uart < 0 || uart >= UART_NUMOF) {
        return -1;
    }

    /* get base register of Sercom device */
    dev = _uart(uart);
    /* calculate baud rate */
    baud =  ((((uint32_t)GCLK_REF * 10) / baudrate) / 16);
    /* save context */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].tx_cb = tx_cb;
    uart_ctx[uart].arg = arg;
    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init_mux(uart_config[uart].rx_pin, GPIO_MUX_D);
    gpio_init(uart_config[uart].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_mux(uart_config[uart].tx_pin, GPIO_MUX_D);
    /* power on the Sercom device */
    uart_poweron(uart);
    /* reset the Sercom device */
    dev->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST);
    /* set asynchronous mode w/o parity, LSB first, PAD0 to TX, PAD1 to RX and
     * use internal clock */
    dev->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                       SERCOM_USART_CTRLA_RXPO(0x1) |
                       SERCOM_USART_CTRLA_SAMPR(0x1) |
                       SERCOM_USART_CTRLA_MODE(0x1));
    /* set baudrate */
    dev->BAUD.FRAC.FP = (baud % 10);
    dev->BAUD.FRAC.BAUD = (baud / 10);
    /* enable receiver and transmitter, use 1 stop bit */
    dev->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB);
    /* finally, enable the device */
    dev->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    /* configure interrupts and enable RX interrupt */
    NVIC_EnableIRQ(SERCOM0_IRQn + _sercom_id(dev));
    dev->INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    _uart(uart)->INTENSET.reg = SERCOM_USART_INTENSET_TXC;
}

void uart_write(uart_t uart, char data)
{
    _uart(uart)->DATA.reg = (uint8_t)data;
}

void uart_write_blocking(uart_t uart, char data)
{
    while (!(_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    _uart(uart)->DATA.reg = (uint8_t)data;
}

void uart_poweron(uart_t uart)
{
    MCLK->APBCMASK.reg |= (MCLK_APBCMASK_SERCOM0 << _sercom_id(_uart(uart)));
    GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(uart))].reg =
                                (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0);
    while (!(GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(uart))].reg &
             GCLK_PCHCTRL_CHEN));
}

void uart_poweroff(uart_t uart)
{
    MCLK->APBCMASK.reg &= ~(MCLK_APBCMASK_SERCOM0 << _sercom_id(_uart(uart)));
    GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(uart))].reg &=
                                                        ~(GCLK_PCHCTRL_CHEN);
}

static inline void irq_handler(int uart)
{
    SercomUsart *dev = _uart(uart);

    if (dev->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, (char)(dev->DATA.reg));
    }
    else if (dev->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
        if (uart_ctx[uart].tx_cb(uart_ctx[uart].arg) == 0) {
            /* disable the TXC interrupt */
            dev->INTENCLR.reg = SERCOM_USART_INTENCLR_TXC;
        }
    }
    else if (dev->INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) {
        /* clear error flag */
        dev->INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}



#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(4);
}
#endif

#ifdef UART_5_ISR
void UART_5_ISR(void)
{
    irq_handler(5);
}
#endif
