/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup        cpu_cc3200
 * @{
 *
 * @file
 * @brief           Driver for the cc3200 UART controller
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 */
#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"
#include "xtimer.h"

#include "vendor/hw_uart.h"
#include "vendor/rom.h"

/* Bit masks for the UART Masked Interrupt Status (MIS) Register: */
#define OEMIS (1 << 10) /**< UART overrun errors */
#define BEMIS (1 << 9)  /**< UART break error */
#define FEMIS (1 << 7)  /**< UART framing error */
#define RTMIS (1 << 6)  /**< UART RX time-out */
#define RXMIS (1 << 4)  /**< UART RX masked interrupt */

/* PIN_MODE value for using a PIN for UART */
#define PIN_MODE_UART 0x00000003

/* guard file in case no UART device was specified */
#if UART_NUMOF

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);

    volatile cc3200_uart_t *reg = uart_config[uart].dev;

    /* get masked interrupt flags */
    uint16_t mis = uart_config[uart].dev->MIS.raw;
    /* clear the interrupt */
    reg->ICR = mis;

    /* read data */
    while (uart_config[uart].dev->flags.bits.RXFE == 0) {
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, uart_config[uart].dev->dr);
    }

    if (mis & (OEMIS | BEMIS | FEMIS)) {
        /* clear error status */
        reg->cc3200_uart_dr.ecr = 0xFF;
    }

    cortexm_isr_end();
}

#if UART_0_ISR
void isr_uart0(void)
{
    irq_handler((uart_t)0);
}
#endif
#if UART_1_ISR
void isr_uart1(void)
{
    irq_handler((uart_t)1);
}
#endif /* UART_1_EN */

/**
 * @brief gracefully disable uart device
 *
 * @param uart interface number
 */
void uart_disable(uart_t uart)
{
    volatile cc3200_uart_t *reg = uart_config[uart].dev;

    /* wait for uart to finish */
    while (reg->flags.bits.BUSY) {}

    /* disable fifo */
    reg->LCRH.bits.FEN = 0;

    /* disable the uart */
    reg->CTL.raw &= ~(UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
}

/**
 * @brief enable uart device
 *
 * @param uart interface number
 */
void uart_enable(uart_t uart)
{
    volatile cc3200_uart_t *reg = uart_config[uart].dev;

    /* enable FIFO */
    reg->LCRH.bits.FEN = 1;

    /* enable TX, RX and UART */
    reg->CTL.raw |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
}

/**
 * @brief configure uart device
 *
 * @param uart interface number
 * @param baudrate
 * @param config
 */
void uart_set_config(uart_t uart, uint32_t baudrate, uint32_t config)
{
    /* stop uart */
    uart_disable(uart);

    volatile cc3200_uart_t *reg = uart_config[uart].dev;
    uint32_t div;
    /* check if baudrate is too high and needs high speed mode */
    if ((baudrate * 16) > CLOCK_CORECLOCK) {
        reg->CTL.bits.HSE = 1;

        /* half the baudrate to compensate for high speed mode */
        baudrate /= 2;
    }
    else {
        /* disable high speed mode */
        reg->CTL.bits.HSE = 0;
    }

    /* compute & set fractional baud rate divider */
    div = (((CLOCK_CORECLOCK * 8) / baudrate) + 1) / 2;
    reg->IBRD = div / 64;
    reg->FBRD = div % 64;

    /* set config */
    reg->LCRH.raw = config;

    /* clear flags */
    reg->flags.raw = 0;

    uart_enable(uart);
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    uart_poweron(uart);

    /* Configure PIN_55 for UART0 UART0_TX */
    ROM_PinTypeUART(uart_config[uart].pin_tx, PIN_MODE_UART);

    /* Configure PIN_57 for UART0 UART0_RX */
    ROM_PinTypeUART(uart_config[uart].pin_rx, PIN_MODE_UART);

    uart_set_config(uart, baudrate,
                    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                     UART_CONFIG_PAR_NONE));

    return 0;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = uart_init_blocking(uart, baudrate);

    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;
    uint32_t uart_base = (uint32_t)uart_config[uart].dev;

    ROM_UARTIntEnable(uart_base, UART_INT_RX | UART_INT_OE | UART_INT_BE |
                      UART_INT_PE | UART_INT_FE);

    ROM_IntPrioritySet(uart_config[uart].irqn, INT_PRIORITY_LVL_3);
    ROM_IntEnable(uart_config[uart].irqn);
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    uint32_t u = (uint32_t)uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        ROM_UARTCharPut(u, data[i]);
    }
}

int uart_read_blocking(uart_t uart, uint8_t *data)
{
    uint32_t u = (uint32_t)uart_config[uart].dev;

    *data = ROM_UARTCharGet(u);

    return 1;
}

int uart_write_blocking(uart_t uart, uint8_t data)
{
    ROM_UARTCharPut((uint32_t)uart_config[uart].dev, data);
    return 1;
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            /* reset & enable periph clk */
            reset_periph_clk(&ARCM->UART_A0);
            ARCM->UART_A0.clk_gating |= PRCM_RUN_MODE_CLK;

            break;
#endif
#if UART_1_EN
        case UART_1:
            /* reset & enable periph clk */
            reset_periph_clk(&ARCM->UART_A1);
            ARCM->UART_A1.clk_gating |= PRCM_RUN_MODE_CLK;
            break;
#endif

        default:
            return;
    }

    uart_enable(uart);
}

void uart_poweroff(uart_t uart)
{
    /* disable uart */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            ARCM->UART_A0.clk_gating &= ~PRCM_RUN_MODE_CLK;

            break;
#endif
#if UART_1_EN
        case UART_1:
            ARCM->UART_A1.clk_gating &= ~PRCM_RUN_MODE_CLK;
            break;
#endif

        default:
            return;
    }
    uart_disable(uart);
}

#endif /* UART_NUMOF */
