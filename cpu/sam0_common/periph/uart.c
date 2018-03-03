/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dylan Laduranty <dylanladuranty@gmail.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

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
static inline SercomUsart *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* must disable here first to ensure idempotency */
    dev(uart)->CTRLA.reg &= ~(SERCOM_USART_CTRLA_ENABLE);

    /* configure pins */
    if (uart_config[uart].rx_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN);
        gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    }
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    gpio_set(uart_config[uart].tx_pin);
    gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);

    /* enable peripheral clock */
    sercom_clk_en(dev(uart));

    /* reset the UART device */
    dev(uart)->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (dev(uart)->SYNCBUSY.bit.SWRST) {}

    /* configure clock generator */
    sercom_set_gen(dev(uart), uart_config[uart].gclk_src);

    /* set asynchronous mode w/o parity, LSB first, TX and RX pad as specified
     * by the board in the periph_conf.h, x16 sampling and use internal clock */
    dev(uart)->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                            SERCOM_USART_CTRLA_SAMPR(0x1) |
                            SERCOM_USART_CTRLA_TXPO(uart_config[uart].tx_pad) |
                            SERCOM_USART_CTRLA_RXPO(uart_config[uart].rx_pad) |
                            SERCOM_USART_CTRLA_MODE(0x1));
    /* Set run in standby mode if enabled */
    if (uart_config[uart].flags & UART_FLAG_RUN_STANDBY) {
        dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_RUNSTDBY;
    }

    /* calculate and set baudrate */
    uint32_t baud = ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);
    dev(uart)->BAUD.FRAC.FP = (baud % 10);
    dev(uart)->BAUD.FRAC.BAUD = (baud / 10);

    /* enable transmitter, and configure 8N1 mode */
    dev(uart)->CTRLB.reg = SERCOM_USART_CTRLB_TXEN;
    /* enable receiver and RX interrupt if configured */
    if ((rx_cb) && (uart_config[uart].rx_pin != GPIO_UNDEF)) {
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
        NVIC_EnableIRQ(SERCOM0_IRQn + sercom_id(dev(uart)));
        dev(uart)->CTRLB.reg |= SERCOM_USART_CTRLB_RXEN;
        dev(uart)->INTENSET.reg |= SERCOM_USART_INTENSET_RXC;
        /* set wakeup receive from sleep if enabled */
        if (uart_config[uart].flags & UART_FLAG_WAKEUP) {
            dev(uart)->CTRLB.reg |= SERCOM_USART_CTRLB_SFDE;
        }
    }
    while (dev(uart)->SYNCBUSY.bit.CTRLB) {}

    /* and finally enable the device */
    dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!dev(uart)->INTFLAG.bit.DRE) {}
        dev(uart)->DATA.reg = data[i];
    }
    while (!dev(uart)->INTFLAG.bit.TXC) {}
}

void uart_poweron(uart_t uart)
{
    sercom_clk_en(dev(uart));
    dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_poweroff(uart_t uart)
{
    dev(uart)->CTRLA.reg &= ~(SERCOM_USART_CTRLA_ENABLE);
    sercom_clk_dis(dev(uart));
}

static inline void irq_handler(unsigned uartnum)
{
    if (dev(uartnum)->INTFLAG.bit.RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uartnum].rx_cb(uart_ctx[uartnum].arg,
                                (uint8_t)(dev(uartnum)->DATA.reg));
    }
    else if (dev(uartnum)->INTFLAG.bit.ERROR) {
        /* clear error flag */
        dev(uartnum)->INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
    }

    cortexm_isr_end();
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
