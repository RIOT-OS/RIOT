/*
 * SPDX-FileCopyrightText: 2021-2023 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 *
 * Supports runtime calculation of the BSEL and BSCALE register values.
 * Supports reconfiguring UART after a clock change.
 *
 * @}
 */
#include <avr/io.h>

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "cpu_pm.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   UART Baudrate Tolerance
 *
 * The tolerance is expressed as baud rate percentage multiplied by 100.
 * For bigger tolerances UART double frequency can be avoided, thus saving
 * power.
 *
 * The default baud tolerance is 2%.
 */
#ifndef BAUD_TOL
#define BAUD_TOL 2 * 100
#endif

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given USART device
 *
 * @param[in] dev       USART device identifier
 *
 * @return              base register address
 */
static inline USART_t *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

static inline int8_t _check_bsel(uint32_t baud, uint32_t calcbaud,
                                 int16_t *precision)
{
    /* Avoid negative values and with precision of two positions
     * after the decimal point for the deviation in percent
     *
     * result is the absolute deviation eg. 10 001
     *
     * MAX BAUD fper/2 = 16MHz a shift of 8 can be used to increase
     * accuracy
     */
    uint16_t pre;

    if (baud < calcbaud) {
        pre = (uint16_t)((calcbaud * 10000ULL) / baud);
    }
    else {
        pre = (uint16_t)((baud * 10000ULL) / calcbaud);
    }

    if (pre < ((uint16_t)*precision)) {
        *precision = pre;
        return 1;
    }

    return 0;
}

static inline int16_t _xmega_bsel_bscale(uint32_t *fper, uint32_t *baud,
                                         uint8_t clk2x, uint16_t *bsel,
                                         int8_t *bscale)
{
    uint32_t calcbaud = 0;
    uint32_t deno = 0;
    int16_t precision = UINT_MAX;
    uint16_t loc_bsel = 0;
    int8_t loc_bscale;

    /* Some explanation for equation transformation
     * bsel =  ( (fper / (2^bscale*16*baud) ) - 1 )
     *      =  ( (fper / (2^bscale*(16-8*clk2x)*baud) ) - 1 )
     *      =  (  fper - (2^bscale*(16-8*clk2x)*baud) )
     *                 / (2^bscale*(16-8*clk2x)*baud)
     *
     * deno = ( baud * (16-8*clk2x)   * 2^bscale    )
     *      = ( baud * (1<<(4-clk2x)) * (1<<bscale) )
     *      = ( baud<<( 4-clk2x +bscale));
     *
     * some rounding math explanation for unsigned integer
     * ABS(x +0,5)  = (x*10+ 5 )/(10)
     * Equation above is as follows, using denominator as base to round
     * bsel = ( x    - y )/y
     *      = ( x    - y +0,5*y )/y
     *      = ( 2*x  - y )/(2*y)
     *      = ((x<<1)- y )/(y<<1)
     *
     * baud = (fper*1/2^bscale) / ((bsel+1)*16)
     */
    for (loc_bscale = 0; loc_bscale <= 7; loc_bscale++) {
        int32_t sub = 0;

        deno = (*baud << (4 - clk2x + loc_bscale));
        sub = (*fper << 1) - (deno);

        if (sub <= 0) {
            break;
        }

        loc_bsel = (sub / (deno << 1));

        if (loc_bsel >= 4095) {
            continue;
        }

        /* Omit division by 16 get higher accuracy at small baudrates*/
        calcbaud = (*fper >> loc_bscale) / ((loc_bsel + 1) << (4 - clk2x));

        if (_check_bsel(*baud, calcbaud, &precision)) {
            *bsel = loc_bsel;
            *bscale = loc_bscale;
        }
    }

    /* More math for the negative equation
     * bscale is negative so 1/2^bscale = 2^|bscale| which is a factor and
     * not a division again runding the result before division with
     * 0.5 * denominator
     *
     * bsel = 1/2^bscale *( fcpu / ( (16*baud)-1) )
     *      = (  fcpu*2^|bscale| - (16*baud)*2^|bscale| )/(16*baud)
     *      = (  fcpu*2^|bscale| - (16*baud)*2^|bscale| + 0.5*(16*baud) )/(16*baud)
     *
     * deno  =  (16/2*baud) = (baud<<(3-clk2x))
     *
     * bsel = (  (fcpu - (deno<<(1))<<|bscale|) + deno )/(deno<<1)
     *
     * Baud = (fper*1/2^bscale)/ (16*(bsel+1/2^bscale))
     */
    for (loc_bscale = -1; loc_bscale >= -7; loc_bscale--) {
        uint32_t num = 0;

        deno = (*baud << (3 - clk2x));
        num = ((*fper - (deno << 1)) << (-loc_bscale)) + deno;
        num = (num / (deno << 1));

        if (num >= 4095) {
            break;
        }

        loc_bsel = (uint16_t)(num & 0xFFF);

        /* Omit division by 16 get higher accuracy at small baudrates */
        calcbaud = (*fper << (-loc_bscale))
                 / ((loc_bsel + (1 << (-loc_bscale))) << (4 - clk2x));

        if (_check_bsel(*baud, calcbaud, &precision)) {
            *bsel = loc_bsel;
            *bscale = loc_bscale;
        }
    }

    return precision;
}

/**
 * @brief   Calculates bsel and bscale for a given periphery clock and baudrate.
 *          Limitation are the periphery clock maximum is 32MHz, unsigned int
 *          overflows if clock is bigger. And the periphery clock has to be not
 *          smaller then 1 when divided by 128.
 *
 * fper/128 !=0 must be divide able by 128
 * fper*128 != uint23_max => 32MHz max fper
 */
static inline int16_t _xmega_calculate_bsel_bscale(uint32_t fcpu, uint32_t baud,
                                                   uint8_t *clk2x,
                                                   uint16_t *bsel,
                                                   int8_t *bscale)
{
    int16_t precision = 0;

    precision = _xmega_bsel_bscale(&fcpu, &baud, 0, bsel, bscale );

    /* default 2% precision, required precision is at least 2% */
    if (precision <= (10000 + BAUD_TOL)) {
        return (precision - 10000);
    }

    /* Precision goal was not met calculate baudrate with uart frequency doubled */
    precision = _xmega_bsel_bscale(&fcpu, &baud, 1, bsel, bscale );
    *clk2x = 1;

    return (precision - 10000);
}

static inline void _configure_pins(uart_t uart)
{
    /* configure RX pin */
    if (gpio_is_valid(uart_config[uart].rx_pin)) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    }

    /* configure TX pin */
    if (gpio_is_valid(uart_config[uart].tx_pin)) {
        gpio_set(uart_config[uart].tx_pin);
        gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    }

#ifdef MODULE_PERIPH_UART_HW_FC
    /* TODO */
#endif
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    int8_t bscale;
    uint8_t clk2x;
    uint16_t bsel;

    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    uint16_t count = UINT16_MAX;
    while (avr8_is_uart_tx_pending() && count--) {}

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg = arg;

    pm_periph_enable(uart_config[uart].pwr);

    /* disable and reset UART */
    dev(uart)->CTRLA = 0;
    dev(uart)->CTRLB = 0;
    dev(uart)->CTRLC = 0;

    _configure_pins(uart);

    /* configure UART to 8N1 mode */
    dev(uart)->CTRLC = USART_CMODE_ASYNCHRONOUS_gc
                     | USART_PMODE_DISABLED_gc
                     | USART_CHSIZE_8BIT_gc;

    /* set clock divider */
    _xmega_calculate_bsel_bscale(CLOCK_CORECLOCK, baudrate, &clk2x,
                                 &bsel, &bscale);

    dev(uart)->BAUDCTRLA = (uint8_t)(bsel & 0x00ff);
    dev(uart)->BAUDCTRLB = (bscale << USART_BSCALE_gp)
                         | ((uint8_t)((bsel & 0x0fff) >> 8));
    if (clk2x == 1) {
        dev(uart)->CTRLB |= USART_CLK2X_bm;
    }

    /* enable RX and TX Interrupts and set level*/
    if (rx_cb) {
        dev(uart)->CTRLA = (uart_config[uart].rx_int_lvl << USART_RXCINTLVL_gp)
                         | (uart_config[uart].tx_int_lvl << USART_TXCINTLVL_gp)
                         | (uart_config[uart].dre_int_lvl << USART_DREINTLVL_gp);
        dev(uart)->CTRLB = USART_RXEN_bm | USART_TXEN_bm;
    }
    else {
        /* only transmit */
        dev(uart)->CTRLB = USART_TXEN_bm;
    }

    DEBUG("Set clk2x %" PRIu8 " bsel %" PRIu16 "bscale %" PRIi8 "\n",
          clk2x, bsel, bscale);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(dev(uart)->STATUS & USART_DREIF_bm)) {}

        /* start of TX won't finish until no data in DATAn and transmit shift
           register is empty */
        uint8_t irq_state = irq_disable();
        avr8_uart_tx_set_pending(uart);
        irq_restore(irq_state);

        dev(uart)->DATA = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    pm_periph_enable(uart_config[uart].pwr);
}

void uart_poweroff(uart_t uart)
{
    pm_periph_disable(uart_config[uart].pwr);
}

static inline void _rx_isr_handler(int num)
{
    if (isr_ctx[num].rx_cb) {
        isr_ctx[num].rx_cb(isr_ctx[num].arg, dev(num)->DATA);
    }
}

static inline void _tx_isr_handler(int num)
{
    /* entire frame in the Transmit Shift Register has been shifted out and
       there are no new data currently present in the transmit buffer */
    avr8_uart_tx_clear_pending(num);
}

#ifdef UART_0_RXC_ISR
AVR8_ISR(UART_0_RXC_ISR, _rx_isr_handler, 0);
AVR8_ISR(UART_0_TXC_ISR, _tx_isr_handler, 0);
#endif /* UART_0_ISR */

#ifdef UART_1_RXC_ISR
AVR8_ISR(UART_1_RXC_ISR, _rx_isr_handler, 1);
AVR8_ISR(UART_1_TXC_ISR, _tx_isr_handler, 1);
#endif /* UART_1_ISR */

#ifdef UART_2_RXC_ISR
AVR8_ISR(UART_2_RXC_ISR, _rx_isr_handler, 2);
AVR8_ISR(UART_2_TXC_ISR, _tx_isr_handler, 2);
#endif /* UART_2_ISR */

#ifdef UART_3_RXC_ISR
AVR8_ISR(UART_3_RXC_ISR, _rx_isr_handler, 3);
AVR8_ISR(UART_3_TXC_ISR, _tx_isr_handler, 3);
#endif /* UART_3_ISR */

#ifdef UART_4_RXC_ISR
AVR8_ISR(UART_4_RXC_ISR, _rx_isr_handler, 4);
AVR8_ISR(UART_4_TXC_ISR, _tx_isr_handler, 4);
#endif /* UART_4_ISR */

#ifdef UART_5_RXC_ISR
AVR8_ISR(UART_5_RXC_ISR, _rx_isr_handler, 5);
AVR8_ISR(UART_5_TXC_ISR, _tx_isr_handler, 5);
#endif /* UART_5_ISR */

#ifdef UART_6_RXC_ISR
AVR8_ISR(UART_6_RXC_ISR, _rx_isr_handler, 6);
AVR8_ISR(UART_6_TXC_ISR, _tx_isr_handler, 6);
#endif /* UART_6_ISR */

#ifdef UART_7_RXC_ISR
AVR8_ISR(UART_7_RXC_ISR, _rx_isr_handler, 7);
AVR8_ISR(UART_7_TXC_ISR, _tx_isr_handler, 7);
#endif /* UART_7_ISR */
