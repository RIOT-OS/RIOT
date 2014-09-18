/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/uart.h"
#include "periph_conf.h"
#include "port_util.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];
static uint64_t long_division(uint64_t n, uint64_t d)
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

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = uart_init_blocking(uart, baudrate);
    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
        case UART_0:
            UART_0_DEV.INTENSET.bit.RXC = 1;
        break;
    }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    /* Calculate the BAUD value */
    GCLK_CLKCTRL_Type clkctrl_core;
    GCLK_CLKCTRL_Type clkctrl_slow;
    uint64_t temp1 = ((16 * ((uint64_t)baudrate)) << 32);
    uint64_t ratio = long_division(temp1 , F_REF);
    uint64_t scale = ((uint64_t)1 << 32) - ratio;
    uint64_t baud_calculated = (65536 * scale) >> 32;
    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* Turn on power manager for sercom */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;
            /*Set up clock generator mux for sercom0*/
            clkctrl_core.bit.ID = SERCOM0_GCLK_ID_CORE;
            clkctrl_core.bit.GEN = 0; //Generator 0
            clkctrl_core.bit.CLKEN = 1;
            clkctrl_core.bit.WRTLOCK = 0;

            clkctrl_slow.bit.ID = SERCOM0_GCLK_ID_SLOW;
            clkctrl_slow.bit.GEN = 0; //Generator 0
            clkctrl_slow.bit.CLKEN = 1;
            clkctrl_slow.bit.WRTLOCK = 0;

            GCLK->CLKCTRL = clkctrl_core;
            while(GCLK->STATUS.bit.SYNCBUSY);
            GCLK->CLKCTRL = clkctrl_slow;
            while(GCLK->STATUS.bit.SYNCBUSY);

            /* configure PINS to input/output*/
            UART_0_PORT.DIRSET.reg = (1 << UART_0_TX_PIN);
            UART_0_PORT.DIRSET.reg = (1 << UART_0_RX_PIN);
            UART_0_PORT.PINCFG[UART_0_RX_PIN % 32].bit.INEN = true;
            UART_0_PORT.PINCFG[UART_0_TX_PIN % 32].bit.INEN = true;
            pmux_set(UART_0_TX_PIN, D);
            pmux_set(UART_0_RX_PIN, D);

            UART_0_DEV.CTRLA.bit.ENABLE = 0; //Disable to write, need to sync tho
            while(UART_0_DEV.SYNCBUSY.bit.ENABLE);
            SERCOM_USART_CTRLA_Type ctrla = 
            {
                .bit.MODE = SERCOM_USART_CTRLA_MODE_USART_INT_CLK_Val,
                .bit.RXPO = 1, //RX = SERCOMPAD[1]
                .bit.TXPO = 0, //TX = SERCOMPAD[0]
                .bit.RUNSTDBY = 0,
                .bit.CMODE = 0, //Async mode
                .bit.DORD = 1, //LSB transmitted first
                .bit.FORM = 0 //USART frame without parity
            };
            UART_0_DEV.CTRLA = ctrla;

            /* Set baud rate */
            UART_0_DEV.BAUD.bit.BAUD = baud_calculated;

            SERCOM_USART_CTRLB_Type ctrlb = 
            {
                .bit.CHSIZE = 0, //8 bit size
                .bit.PMODE = 0, //Parity even
                .bit.SBMODE = 0, //1 stop bit
                .bit.RXEN = 1,
                .bit.TXEN = 1
            };
            UART_0_DEV.CTRLB = ctrlb;
            while(UART_0_DEV.SYNCBUSY.bit.CTRLB);

            uart_poweron(uart);

            return 1;
            break;
#endif
    }

    uart_poweron(uart);
    return 0;
}

void uart_tx_begin(uart_t uart)
{

}

void uart_tx_end(uart_t uart)
{

}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            UART_0_DEV.DATA.reg = (uint8_t)*((uint8_t*)&data);
            break;
    }
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
        case UART_0:
            while (UART_0_DEV.INTFLAG.bit.RXC == 0);
            *data = (char)(0x00ff & UART_0_DEV.DATA.reg);
            break;
    }
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            while (UART_0_DEV.INTFLAG.bit.DRE == 0);
            UART_0_DEV.DATA.reg = *((uint8_t*)&data);
            break;
    }
    return 1;
}

void uart_poweron(uart_t uart)
{
    while (UART_0_DEV.SYNCBUSY.reg);
    UART_0_DEV.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_poweroff(uart_t uart)
{
    while (UART_0_DEV.SYNCBUSY.reg);

    UART_0_DEV.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
}

#endif /* UART_NUMOF */
