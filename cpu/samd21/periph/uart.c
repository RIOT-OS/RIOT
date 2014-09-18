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
	uint16_t clock_divider = (0xffff & (65536 * ((uint64_t)1 << 32) - (((16 * (uint64_t)9600) << 32)/8000000)) >> 32);

    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* sercom clk enable */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

            /* configure GCLK0 to feed sercom0 */
            GCLK->CLKCTRL.reg &= (uint16_t)(~GCLK_CLKCTRL_CLKEN);
            GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (0x14 << GCLK_CLKCTRL_ID_Pos)));

            uart_poweroff(uart);
            /* reset */
            UART_0_DEV.CTRLA.reg = 0;

            while (UART_0_DEV.SYNCBUSY.reg);

            /* set clock divider */
            UART_0_DEV.BAUD.reg = clock_divider;

            while (UART_0_DEV.SYNCBUSY.reg);

            /* enable receiver and transmitter, one stop bit*/
            UART_0_DEV.CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);

            while (UART_0_DEV.SYNCBUSY.reg);

            /* set to LSB, asynchronous mode without parity, PAD0 Tx, PAD1 Rx,
             * 16x over-sampling, internal clk */
            UART_0_DEV.CTRLA.reg = SERCOM_USART_CTRLA_DORD \
                                    | SERCOM_USART_CTRLA_FORM(0x0) \
                                    | SERCOM_USART_CTRLA_SAMPA(0x0) \
                                    | SERCOM_USART_CTRLA_TXPO(0x0) \
                                    | SERCOM_USART_CTRLA_RXPO(0x1) \
                                    | SERCOM_USART_CTRLA_SAMPR(0x0) \
                                    | SERCOM_USART_CTRLA_MODE_USART_INT_CLK;

            /* configure PINS */
            UART_0_PORT.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG \
                                        | PORT_WRCONFIG_WRPMUX \
                                        | PORT_WRCONFIG_PMUX(0x3) \
                                        | PORT_WRCONFIG_PMUXEN
                                        | UART_0_PINS;
            /* PA04 output */
            UART_0_PORT.DIRSET.reg = PORT_PA04;
            /* PA05 input buffered */
            UART_0_PORT.PINCFG[PIN_PA05].bit.INEN=1;

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
