/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief   Each UART device has to store two callbacks
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_isr_ctx_t;

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

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

int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = uart_init_blocking(uart, baudrate);
    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].tx_cb = tx_cb;
    uart_ctx[uart].arg = arg;
    /* configure interrupts and enable RX interrupt */
    _uart(uart)->INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    NVIC_EnableIRQ(SERCOM0_IRQn + _sercom_id(_uart(uart)));
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    uint32_t baud;
    SercomUsart *dev;

    if (uart < 0 || uart >= UART_NUMOF) {
        return -1;
    }

    /* get the devices base register */
    dev = _uart(uart);
    /* calculate baudrate */
    baud =  ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);
    /* enable sync and async clocks */
    uart_poweron(uart);
    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    gpio_init(uart_config[uart].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);
    /* reset the UART device */
    dev->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST);
    /* set asynchronous mode w/o parity, LSB first, PAD0 to TX, PAD1 to RX and
     * use internal clock */
    dev->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                      SERCOM_USART_CTRLA_RXPO(0x1) |
                      SERCOM_USART_CTRLA_SAMPR(0x1) |
                      SERCOM_USART_CTRLA_MODE_USART_INT_CLK);
    /* set baudrate */
    dev->BAUD.FRAC.FP = (baud % 10);
    dev->BAUD.FRAC.BAUD = (baud / 10);
    /* enable receiver and transmitter, use 1 stop bit */
    dev->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB);
    /* finally, enable the device */
    dev->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    _uart(uart)->INTENSET.reg = SERCOM_USART_INTENSET_TXC;
}

int uart_read_blocking(uart_t uart, char *data)
{
    while(!(_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC));
    *data = (char)_uart(uart)->DATA.reg;
    return 1;
}

int uart_write(uart_t uart, char data)
{
    _uart(uart)->DATA.reg = (uint8_t)data;
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    while (!(_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    _uart(uart)->DATA.reg = (uint8_t)data;
    return 1;
}

void uart_poweron(uart_t uart)
{
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << _sercom_id(_uart(uart)));
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK0 |
                         (SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(uart))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}

void uart_poweroff(uart_t uart)
{
    PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 << _sercom_id(_uart(uart)));
    GCLK->CLKCTRL.reg = ((SERCOM0_GCLK_ID_CORE + _sercom_id(_uart(uart))) <<
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
        if (uart_ctx[dev].tx_cb(uart_ctx[dev].arg) == 0) {
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
