/*
 * Copyright (C) 2015 Freie Universität Berlin
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
static inline SercomUsart *_uart(uart_t dev)
{
    return uart_config[dev].dev;
}

#ifdef CPU_FAM_SAML21

static uint64_t _long_division(uint64_t n, uint64_t d);

/**
 * @brief   Get the sercom gclk id of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              sercom gclk id
 */

static int _get_sercom_gclk_id(SercomUsart *dev)
{
    int id;

    switch(sercom_id(dev)) {
        case 0:
            id = SERCOM0_GCLK_ID_CORE;
            break;
        case 1:
            id = SERCOM1_GCLK_ID_CORE;
            break;
        case 2:
            id = SERCOM2_GCLK_ID_CORE;
            break;
        case 3:
            id = SERCOM3_GCLK_ID_CORE;
            break;
        case 4:
            id = SERCOM4_GCLK_ID_CORE;
            break;
        case 5:
            id = SERCOM5_GCLK_ID_CORE;
            break;
        default:
            id = -1;
            DEBUG("[UART]: wrong SERCOM ID\n");
            break;
    }
    return id;
}
#endif

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return res;
    }

    /* register callbacks */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;
    /* configure interrupts and enable RX interrupt */
    NVIC_EnableIRQ(SERCOM0_IRQn + sercom_id(_uart(uart)));
    _uart(uart)->INTENSET.reg |= SERCOM_USART_INTENSET_RXC;

    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    SercomUsart *dev;

    if ((unsigned int)uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* get the devices base register */
    dev = _uart(uart);

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);

#ifdef CPU_FAM_SAMD21
    /* calculate baudrate */
    uint32_t  baud =  ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);
    /* enable sync and async clocks */
    uart_poweron(uart);
    /* reset the UART device */
    dev->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST) {}
    /* set asynchronous mode w/o parity, LSB first, TX and RX pad as specified
     * by the board in the periph_conf.h, x16 sampling and use internal clock */
    dev->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                      SERCOM_USART_CTRLA_SAMPR(0x1) |
                      SERCOM_USART_CTRLA_TXPO(uart_config[uart].tx_pad) |
                      SERCOM_USART_CTRLA_RXPO(uart_config[uart].rx_pad) |
                      SERCOM_USART_CTRLA_MODE_USART_INT_CLK             |
                      (uart_config[uart].runstdby ?
                              SERCOM_USART_CTRLA_RUNSTDBY : 0));

    /* set baudrate */
    dev->BAUD.FRAC.FP = (baud % 10);
    dev->BAUD.FRAC.BAUD = (baud / 10);
    /* enable receiver and transmitter, use 1 stop bit */
    dev->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while (dev->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB) {}
#elif CPU_FAM_SAML21
    /* Calculate the BAUD value */
    uint64_t temp1 = ((16 * ((uint64_t)baudrate)) << 32);
    uint64_t ratio = _long_division(temp1 , CLOCK_CORECLOCK);
    uint64_t scale = ((uint64_t)1 << 32) - ratio;
    uint64_t baud_calculated = (65536 * scale) >> 32;

    dev->CTRLA.bit.ENABLE = 0; /* Disable to write, need to sync tho */
    while(dev->SYNCBUSY.bit.ENABLE) {}

    /* set to LSB, asynchronous mode without parity, PAD0 Tx, PAD1 Rx,
     * 16x over-sampling, internal clk */
    dev->CTRLA.reg = SERCOM_USART_CTRLA_DORD \
                    | SERCOM_USART_CTRLA_FORM(0x0) \
                    | SERCOM_USART_CTRLA_SAMPA(0x0) \
                    | SERCOM_USART_CTRLA_TXPO(uart_config[uart].tx_pad) \
                    | SERCOM_USART_CTRLA_RXPO(uart_config[uart].rx_pad) \
                    | SERCOM_USART_CTRLA_SAMPR(0x0) \
                    | SERCOM_USART_CTRLA_MODE(0x1) \
                    | (uart_config[uart].runstdby ?
                            SERCOM_USART_CTRLA_RUNSTDBY : 0);

    /* Set baud rate */
    dev->BAUD.bit.BAUD = baud_calculated;

    /* enable receiver and transmitter, one stop bit*/
    dev->CTRLB.reg = (SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN);
    while(dev->SYNCBUSY.bit.CTRLB) {}
    uart_poweron(uart);
#endif
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {}
        _uart(uart)->DATA.reg = data[i];
        while (_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {}
    }
}

void uart_poweron(uart_t uart)
{
    SercomUsart *dev;

    /* get the devices base register */
    dev = _uart(uart);

#ifdef CPU_FAM_SAMD21
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << sercom_id(_uart(uart)));
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN(uart_config[uart].gclk_src) |
                         (SERCOM0_GCLK_ID_CORE + sercom_id(_uart(uart))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#elif CPU_FAM_SAML21
    /* Enable the peripheral channel */
    GCLK->PCHCTRL[_get_sercom_gclk_id(dev)].reg |= GCLK_PCHCTRL_CHEN |
                                                   GCLK_PCHCTRL_GEN(uart_config[uart].gclk_src);

    while (!(GCLK->PCHCTRL[_get_sercom_gclk_id(dev)].reg & GCLK_PCHCTRL_CHEN)) {}
    if(_get_sercom_gclk_id(dev) < 5) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0 << sercom_id(dev);
    }
    else {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;
    }
    while (dev->SYNCBUSY.reg) {}
#endif
    /* finally, enable the device */
    dev->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_poweroff(uart_t uart)
{
    SercomUsart *dev;

    /* get the devices base register */
    dev = _uart(uart);

#ifdef CPU_FAM_SAMD21
    PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 << sercom_id(dev));
    GCLK->CLKCTRL.reg = ((SERCOM0_GCLK_ID_CORE + sercom_id(dev)) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#elif CPU_FAM_SAML21
    /* Enable the peripheral channel */
    GCLK->PCHCTRL[_get_sercom_gclk_id(dev)].reg &= ~GCLK_PCHCTRL_CHEN;

    if(_get_sercom_gclk_id(dev) < 5) {
        MCLK->APBCMASK.reg &= ~(MCLK_APBCMASK_SERCOM0 << sercom_id(dev));
    }
    else {
        MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM5;
    }
    while (dev->SYNCBUSY.reg) {}

#endif
}

static inline void irq_handler(uint8_t uartnum)
{
    SercomUsart *uart = _uart(uartnum);
#ifdef CPU_FAM_SAMD21
    if (uart->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uartnum].rx_cb(uart_ctx[uartnum].arg, (uint8_t)(uart->DATA.reg));
    }
    else if (uart->INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) {
        /* clear error flag */
        uart->INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
    }
#elif CPU_FAM_SAML21
    if (uart->INTFLAG.bit.RXC) {
        /* cleared by reading DATA regiser */
        uint8_t data = (uint8_t)uart->DATA.reg;
        uart_ctx[uartnum].rx_cb(uart_ctx[uartnum].arg, data);
    }
    else if (uart->INTFLAG.bit.ERROR) {
        /* clear error flag */
        uart->INTFLAG.reg |= SERCOM_USART_INTFLAG_ERROR;
    }
#endif
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

#ifdef CPU_FAM_SAML21
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
#endif
