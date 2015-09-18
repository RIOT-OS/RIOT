/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              base register address
 */
static inline SercomUsart *_uart(uart_t dev)
{
    return uart_config[dev].dev;
}

int uart_init(uart_t dev, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    uint32_t baud;
    SercomUsart *uart;

    /* check if device is valid */
    if (dev < 0 || dev >= UART_NUMOF) {
        return -1;
    }

    /* get the device's base register */
    uart = _uart(dev);
    /* calculate baudrate */
    baud =  ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);
    /* register callbacks */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].tx_cb = tx_cb;
    isr_ctx[dev].arg = arg;
    /* enable sync and async clocks */
    uart_poweron(dev);
    /* configure pins */
    gpio_init(uart_config[dev].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init_mux(uart_config[dev].rx_pin, GPIO_MUX_D);
    gpio_init(uart_config[dev].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_mux(uart_config[dev].tx_pin, GPIO_MUX_D);
    /* reset the UART device */
    uart->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (uart->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST);
    /* set asynchronous mode w/o parity, LSB first, PAD0 to TX, PAD1 to RX and
     * use internal clock */
    uart->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                       SERCOM_USART_CTRLA_RXPO(0x1) |
                       SERCOM_USART_CTRLA_SAMPR(0x1) |
                       SERCOM_USART_CTRLA_MODE_USART_INT_CLK);
    /* set baudrate */
    uart->BAUD.FRAC.FP = (baud % 10);
    uart->BAUD.FRAC.BAUD = (baud / 10);
    /* enable receiver and transmitter, use 1 stop bit */
    uart->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while (uart->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB);
    /* finally, enable the device */
    uart->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    /* configure interrupts and enable RX interrupt */
    NVIC_EnableIRQ(SERCOM0_IRQn + _sercom_id(_uart(dev)));
    uart->INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    return 0;
}
void uart_tx(uart_t dev)
{
    _uart(dev)->INTENSET.reg = SERCOM_USART_INTENSET_TXC;
}

void uart_write_blocking(uart_t dev, char data)
{
    while (!(_uart(dev)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    _uart(dev)->DATA.reg = (uint8_t)data;
}

void uart_poweron(uart_t dev)
{
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << _sercom_id(_uart(dev)));
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK0 |
                         (SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(dev))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}

void uart_poweroff(uart_t dev)
{
    PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 << _sercom_id(_uart(dev)));
    GCLK->CLKCTRL.reg = ((SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(dev))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}


static inline void irq_handler(int dev)
{
    SercomUsart *uart = _uart(dev);

    if (uart->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[dev].rx_cb(uart_ctx[dev].arg, (char)(uart->DATA.reg));
    }
    else if (uart->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
        uint16_t data = isr_ctx[dev].tx_cb(isr_ctx[dev].arg);
        if (data) {
            uart->DATA.reg = (uint8_t)data;
        }
        else {
            /* disable the TXC interrupt */
            uart->INTENCLR.reg = SERCOM_USART_INTENCLR_TXC;
        }
    }
    else if (uart->INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) {
        /* clear error flag */
        uart->INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
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
