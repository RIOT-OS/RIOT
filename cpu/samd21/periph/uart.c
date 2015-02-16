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
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/uart.h"
#include "periph_conf.h"

#include "sched.h"
#include "thread.h"

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
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param uart          the UART device that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, SercomUsart *uart);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];

static uint64_t _long_division(uint64_t n, uint64_t d);

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
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ);
            UART_0_DEV.INTENSET.bit.RXC = 1;
        break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_SetPriority(UART_1_IRQ, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ);
            UART_1_DEV.INTENSET.bit.RXC = 1;
        break;
#endif
    }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    SercomUsart* uart_dev = NULL;
    PortGroup* port_group = NULL;
    uint32_t uart_ref_f = 0;
    uint32_t tx_pin = 0;
    uint32_t rx_pin = 0;
    uint32_t HWSEL = 0;


    switch (uart) {
#if UART_0_EN
        case UART_0:
            port_group = &UART_0_PORT;
            uart_dev = &UART_0_DEV;
            uart_ref_f = UART_0_REF_F;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            HWSEL = 0;

            /* Turn on power manager for sercom0 */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

            /* configure GCLK0 to feed sercom0 */;
            GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM0_GCLK_ID_CORE << GCLK_CLKCTRL_ID_Pos)));
            while (GCLK->STATUS.bit.SYNCBUSY);

            GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM0_GCLK_ID_SLOW << GCLK_CLKCTRL_ID_Pos)));
            while (GCLK->STATUS.bit.SYNCBUSY);

            break;
#endif
#if UART_1_EN
        case UART_1:
            port_group = &UART_1_PORT;
            uart_dev = &UART_1_DEV;
            uart_ref_f = UART_1_REF_F;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            HWSEL = (PORT_WRCONFIG_HWSEL);

            /* Turn on power manager for sercom5 */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM5;

            /* configure GCLK0 to feed sercom5 */;
            GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM5_GCLK_ID_CORE << GCLK_CLKCTRL_ID_Pos)));
            while (GCLK->STATUS.bit.SYNCBUSY);

            GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM5_GCLK_ID_SLOW << GCLK_CLKCTRL_ID_Pos)));
            while (GCLK->STATUS.bit.SYNCBUSY);

            break;
#endif
        default:
            return -2;
    }

    if (port_group == NULL) {
        return -2;
    }

    /* configure PINS to input/output*/
    port_group->DIRSET.reg = (1 << tx_pin);  /* tx's direction is output */

    port_group->PINCFG[rx_pin % 32].bit.DRVSTR = 0; /* pin drive strength is set to normal */
    port_group->PINCFG[tx_pin % 32].bit.DRVSTR = 0;
    port_group->PINCFG[rx_pin % 32].bit.INEN = 1; /* buffer rx pin's value */
    port_group->PINCFG[rx_pin % 32].bit.PMUXEN = 1; /* enables peripheral multiplexer */
    port_group->PINCFG[tx_pin % 32].bit.PMUXEN = 1; /* enables peripheral multiplexer */


    /* enable PMUX for pins and set to config D. See spec p. 12 */
    port_group->WRCONFIG.reg = HWSEL                      /* Upper/lower 16 pins of PORT will be configured */
                                | PORT_WRCONFIG_WRPINCFG  /* PINCFGy registers of the selected pins will be updated */
                                | PORT_WRCONFIG_WRPMUX    /* PMUXn registers of the selected pins will be updated */
                                | PORT_WRCONFIG_PMUX(0x3) /* Multiplexer peripheral function D */
                                | PORT_WRCONFIG_PMUXEN    /* Peripheral multiplexer enable */
                                | (1 << ( tx_pin % 16 ))  /* Pinmask */
                                | (1 << ( rx_pin % 16 )); /* Pinmask */

    uart_dev->CTRLA.bit.ENABLE = 0; /* Disable to write, need to sync to */
    while(uart_dev->SYNCBUSY.bit.ENABLE);

    /* set to LSB, asynchronous mode without parity, PAD0 Tx, PAD1 Rx,
     * 16x over-sampling, internal clk */
    uart_dev->CTRLA.reg = SERCOM_USART_CTRLA_DORD                     /* LSB first */
                            | SERCOM_USART_CTRLA_FORM(0x0)            /* USART frame */
                            | SERCOM_USART_CTRLA_SAMPA(0x0)           /* 16x Oversampling */
                            | SERCOM_USART_CTRLA_TXPO(0x0)            /* PAD[0] */
                            | SERCOM_USART_CTRLA_RXPO(0x1)            /* PAD[1] */
                            | SERCOM_USART_CTRLA_SAMPR(0x0)           /* 16x Oversampling */
                            | SERCOM_USART_CTRLA_MODE_USART_INT_CLK;  /* USART with internal clock */


    /* Calculate the BAUD value */
    uint64_t temp1 = ((16 * ((uint64_t)baudrate)) << 32);
    uint64_t ratio = _long_division(temp1 , uart_ref_f);
    uint64_t scale = ((uint64_t)1 << 32) - ratio;
    uint64_t baud_calculated = (65536 * scale) >> 32;

    /* Set baud rate */
    uart_dev->BAUD.bit.BAUD = baud_calculated;

    /* enable receiver and transmitter, one stop bit*/
    uart_dev->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while(uart_dev->SYNCBUSY.bit.CTRLB);

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
#if UART_0_EN
        case UART_0:
            UART_0_DEV.DATA.reg = (uint8_t)data;
            break;
#endif
#if UART_1_EN
    case UART_1:
            UART_1_DEV.DATA.reg = (uint8_t)data;
            break;
#endif
    }
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            while (UART_0_DEV.INTFLAG.bit.RXC == 0);
            *data = (char)(0x00ff & UART_0_DEV.DATA.reg);
            break;
#endif
#if UART_1_EN
        case UART_1:
            while (UART_1_DEV.INTFLAG.bit.RXC == 0);
            *data = (char)(0x00ff & UART_1_DEV.DATA.reg);
            break;
#endif
    }
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            while (UART_0_DEV.INTFLAG.bit.DRE == 0);
            UART_0_DEV.DATA.reg = (uint8_t)data;
            break;
#endif
#if UART_1_EN
        case UART_1:
            while (UART_1_DEV.INTFLAG.bit.DRE == 0);
            UART_1_DEV.DATA.reg = (uint8_t)data;
            break;
#endif
    }
    return 1;
}

void uart_poweron(uart_t uart)
{
#if UART_0_EN
    while (UART_0_DEV.SYNCBUSY.reg);
    UART_0_DEV.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
#endif
#if UART_1_EN
    while (UART_1_DEV.SYNCBUSY.reg);
    UART_1_DEV.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
#endif
}

void uart_poweroff(uart_t uart)
{
#if UART_0_EN
    while (UART_0_DEV.SYNCBUSY.reg);
    UART_0_DEV.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
#endif
#if UART_1_EN
    while (UART_1_DEV.SYNCBUSY.reg);
    UART_1_DEV.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
#endif
}

#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, &UART_0_DEV);
}
#endif
#if UART_1_EN
void UART_1_ISR(void)
{
    irq_handler(UART_1, &UART_1_DEV);
}
#endif

static inline void irq_handler(uint8_t uartnum, SercomUsart *dev)
{
    if (dev->INTFLAG.bit.RXC) {
        /* cleared by reading DATA regiser */
        char data = (char)dev->DATA.reg;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    else if (dev->INTFLAG.bit.TXC) {
        if (uart_config[uartnum].tx_cb(uart_config[uartnum].arg) == 0) {
            /* TXC flag is also cleared by writing data to DATA register */
            if (dev->INTFLAG.bit.TXC) {
                /* cleared by writing 1 to TXC */
                dev->INTFLAG.bit.TXC = 1;
            }
        }
    }
    else if (dev->INTFLAG.bit.ERROR) {
        /* clear error flag */
        dev->INTFLAG.bit.ERROR = 1;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

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

#endif /* UART_NUMOF */
