/*
 * SPDX-FileCopyrightText: 2008-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Peripheral UART driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cpu.h"
#include "VIC.h"
#include "periph/uart.h"

static uart_rx_cb_t _rx_cb[UART_NUMOF];
static void * _cb_arg[UART_NUMOF];

typedef void (*irq_fun_t)(void);

#if UART_NUMOF > 0
static void UART0_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif
#if UART_NUMOF > 1
static void UART1_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif
#if UART_NUMOF > 2
static void UART2_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif
#if UART_NUMOF > 3
static void UART3_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              base register address
 */
static inline lpc23xx_uart_t *get_dev(uart_t dev)
{
    return uart_config[dev].dev;
}

/* get the UART number from the address */
static inline uint8_t _uart_num(lpc23xx_uart_t* uart)
{
    switch ((uint32_t) uart) {
    case UART0_BASE_ADDR:
        return 0;
    case UART1_BASE_ADDR:
        return 1;
    case UART2_BASE_ADDR:
        return 2;
    case UART3_BASE_ADDR:
        return 3;
    }

    return 0;
}

static const uint8_t _uart_int[] = {
    UART0_INT, UART1_INT, UART2_INT, UART3_INT
};

static irq_fun_t _uart_isr[UART_NUMOF] = {
#if UART_NUMOF > 0
    UART0_IRQHandler,
#endif
#if UART_NUMOF > 1
    UART1_IRQHandler,
#endif
#if UART_NUMOF > 2
    UART2_IRQHandler,
#endif
#if UART_NUMOF > 3
    UART3_IRQHandler,
#endif
};

/* Table for FDR register contents
 * bits 0…3: DIVADDVAL
 * bits 4…7: MULVAL
 */
#define DIV_MUL(d, m) ((m << 4) | d)
static const uint8_t div_table[] = {
    DIV_MUL(0,  1),  /* 1       */
    DIV_MUL(0,  1),  /* 1.03125 */
    DIV_MUL(1, 15),  /* 1.0625  */
    DIV_MUL(1, 11),  /* 1.09375 */
    DIV_MUL(1,  8),  /* 1.125   */
    DIV_MUL(2, 13),  /* 1.15625 */
    DIV_MUL(2, 11),  /* 1.1875  */
    DIV_MUL(2,  9),  /* 1.21875 */
    DIV_MUL(1,  4),  /* 1.25    */
    DIV_MUL(2,  7),  /* 1.21875 */
    DIV_MUL(4, 13),  /* 1.3125  */
    DIV_MUL(1,  3),  /* 1.34375 */
    DIV_MUL(3,  8),  /* 1.375   */
    DIV_MUL(2,  5),  /* 1.40625 */
    DIV_MUL(3,  7),  /* 1.4375  */
    DIV_MUL(7, 15),  /* 1.46875 */
    DIV_MUL(1,  2),  /* 1.5     */
    DIV_MUL(8, 15),  /* 1.53125 */
    DIV_MUL(5,  9),  /* 1.5625  */
    DIV_MUL(3,  5),  /* 1.59375 */
    DIV_MUL(5,  8),  /* 1.625   */
    DIV_MUL(9, 14),  /* 1.65625 */
    DIV_MUL(2,  3),  /* 1.6875  */
    DIV_MUL(5,  7),  /* 1.71875 */
    DIV_MUL(3,  4),  /* 1.75    */
    DIV_MUL(7,  9),  /* 1.78125 */
    DIV_MUL(9, 11),  /* 1.8125  */
    DIV_MUL(11,13),  /* 1.84375 */
    DIV_MUL(7,  8),  /* 1.875   */
    DIV_MUL(9, 13),  /* 1.90625 */
    DIV_MUL(14,15),  /* 1.9375  */
    DIV_MUL(14,15),  /* 1.96875 */
};

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (dev >= UART_NUMOF) {
        return UART_NODEV;
    }

    const uart_conf_t *cfg = &uart_config[dev];
    lpc23xx_uart_t *uart = cfg->dev;
    const uint8_t idx = _uart_num(uart);

    /* configure RX & TX pins */
    *(&PINSEL0 + cfg->pinsel_rx) |= cfg->pinsel_msk_rx;
    *(&PINSEL0 + cfg->pinsel_tx) |= cfg->pinsel_msk_tx;

    uart_poweron(dev);

    /* save interrupt context */
    _rx_cb[dev]  = rx_cb;
    _cb_arg[dev] = arg;

    uart->LCR = 0x80;       /* DLAB = 1 */

    /* set UART PCLK to CCLK/8 */
    switch (idx) {
    case 0:
        PCLKSEL0 |= BIT6 + BIT7;
        break;
    case 1:
        PCLKSEL0 |= BIT8 + BIT9;
        break;
    case 2:
        PCLKSEL1 |= BIT16 + BIT17;
        break;
    case 3:
        PCLKSEL1 |= BIT18 + BIT19;
        break;
    }

    /* PCLK = CCLK/8; DL = PCLK / (16 * F_Baud) */
    uint16_t dl = CLOCK_CORECLOCK / (8 * 16 * baudrate);
    /* 16 * DIVADDVAL / MULVAL = PCLK / (DL * F_Baud) - 16 */
    /* frac = 16 * DIVADDVAL / MULVAL                      */
    /* multiply everything by 2 for increased accuracy     */
    uint8_t frac = 2 * CLOCK_CORECLOCK / (8 * dl) / baudrate - 32;

    uart->FDR = div_table[frac];
    uart->DLM = dl >> 8;
    uart->DLL = dl & 0xFF;

    uart->LCR = 0x03;       /* configure to 8N1 */
    uart->FCR = 0x07;       /* Enable and reset TX and RX FIFO */

    /* install and enable the IRQ handler */
    install_irq(_uart_int[idx], _uart_isr[dev], cfg->irq_prio_rx);
    uart->IER |= BIT0;      /* enable only RX irq */

    return UART_OK;
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    lpc23xx_uart_t *uart = get_dev(dev);

    for (const uint8_t *end = data + len; data != end; ++data) {
        while (!(uart->LSR & ULSR_THRE)) {}
        uart->THR = *data;
    }
}

static void irq_handler(uart_t dev)
{
    lpc23xx_uart_t *uart = get_dev(dev);

    switch (uart->IIR & UIIR_ID_MASK) {
        case UIIR_CTI_INT:              /* Character Timeout Indicator */
        case UIIR_RDA_INT:              /* Receive Data Available */
                do {
                    _rx_cb[dev](_cb_arg[dev], uart->RBR);
                } while (uart->LSR & ULSR_RDR);
            break;

        default:
            uart->LSR;
            uart->RBR;
            break;
    }
}

#if UART_NUMOF > 0
static void UART0_IRQHandler(void)
{
    irq_handler(0);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif

#if UART_NUMOF > 1
static void UART1_IRQHandler(void)
{
    irq_handler(1);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif

#if UART_NUMOF > 2
static void UART2_IRQHandler(void)
{
    irq_handler(2);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif

#if UART_NUMOF > 3
static void UART3_IRQHandler(void)
{
    irq_handler(3);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif

/* Bit in the Power Control for Peripherals Register */
static const uint8_t _uart_pconp[] = {
    3, 4, 24, 25
};

void uart_poweron(uart_t uart)
{
    const uart_conf_t *cfg = &uart_config[uart];
    const uint8_t idx = _uart_num(cfg->dev);

    /* power on the UART device */
    PCONP |= 1 << _uart_pconp[idx];
}

void uart_poweroff(uart_t uart)
{
    const uart_conf_t *cfg = &uart_config[uart];
    const uint8_t idx = _uart_num(cfg->dev);

    /* power off the UART device */
    PCONP &= ~(1 << _uart_pconp[idx]);
}
