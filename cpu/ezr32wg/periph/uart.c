/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
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
static inline USART_TypeDef *_uart(uart_t dev)
{
    return uart_config[dev].dev;
}

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    USART_TypeDef *uart;

    /* check if device is valid and get base register address */
    if (dev >= UART_NUMOF) {
        return -1;
    }
    uart = _uart(dev);

    /* save interrupt callback context */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg = arg;

    /* power on the device */
    uart_poweron(dev);
    /* put device in asynchronous mode @ 16x oversampling (default UART) */
    uart->CTRL = 0;
    /* configure to default 8N1 configuration */
    uart->FRAME = (USART_FRAME_STOPBITS_ONE | USART_FRAME_DATABITS_EIGHT);
    /* configure the baudrate - this looks more complicated than it is, we just
     * multiply the HFPERCLK with 32 to cut down on rounding error when doing
     * the division afterwards... */
    uart->CLKDIV = (((CLOCK_HFPERCLK << 5) / (16 * baudrate) - 32) << 3);
    /* configure the pins */
    gpio_init(uart_config[dev].rx_pin, GPIO_DIR_IN, GPIO_NOPULL);
    gpio_init(uart_config[dev].tx_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    uart->ROUTE = ((uart_config[dev].loc << _USART_ROUTE_LOCATION_SHIFT) |
                   USART_ROUTE_RXPEN | USART_ROUTE_TXPEN);
    /* enable RX interrupt */
    NVIC_EnableIRQ(uart_config[dev].irq);
    NVIC_EnableIRQ(uart_config[dev].irq + 1);
    uart->IEN |= USART_IEN_RXDATAV;
    /* enable receiver and transmitter */
    uart->CMD = USART_CMD_TXEN | USART_CMD_RXEN;
    return 0;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(_uart(dev)->STATUS & USART_STATUS_TXBL));
        _uart(dev)->TXDATA = data[i];
    }
}

void uart_poweron(uart_t dev)
{
    CMU->HFPERCLKEN0 |= (1 << uart_config[dev].cmu);
}

void uart_poweroff(uart_t dev)
{
    CMU->HFPERCLKEN0 &= ~(1 << uart_config[dev].cmu);
}

static inline void rx_irq(int dev)
{
    if (_uart(dev)->IF & USART_IF_RXDATAV) {
        char data = (char)_uart(dev)->RXDATA;
        isr_ctx[dev].rx_cb(isr_ctx[dev].arg, data);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef UART_0_ISR_RX
void UART_0_ISR_RX(void)
{
    rx_irq(0);
}
#endif

#ifdef UART_1_ISR_RX
void UART_1_ISR_RX(void)
{
    rx_irq(1);
}
#endif

#ifdef UART_2_ISR_RX
void UART_2_ISR_RX(void)
{
    rx_irq(2);
}
#endif
