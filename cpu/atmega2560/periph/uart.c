/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "thread.h"
#include "sched.h"

#include "periph/uart.h"
#include "periph_conf.h"

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t config[UART_NUMOF];

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);

    if (res != 0) {
        return res;
    }

    /* register callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
#if UART_0_EN

        case UART_0:
            UART0_RX_IRQ_EN;
            break;
#endif /* UART_0_EN */
#if UART_1_EN

        case UART_1:
            UART1_RX_IRQ_EN;
            break;
#endif /* UART_1_EN */
#if UART_2_EN

        case UART_2:
            UART2_RX_IRQ_EN;
            break;
#endif /* UART_2_EN */
#if UART_3_EN

        case UART_3:
            UART3_RX_IRQ_EN;
            break;
#endif /* UART_3_EN */
    }

    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    uint16_t clock_divider = F_CPU / (16 * baudrate);

    switch (uart) {
#if UART_0_EN

        case UART_0:
            /* enable RX and TX */
            UART0_RX_TX_EN;
            /* use 8 Bit characters */
            UART0_SET_8BIT_SIZE;

            /* set clock divider */
            UART0_BAUD_RATE_L = clock_divider;
            UART0_BAUD_RATE_H = (clock_divider >> 8);
            break;
#endif /* UART_0 */
#if UART_1_EN

        case UART_1:
            /* enable RX and TX */
            UART1_RX_TX_EN;
            /* use 8 Bit characters */
            UART1_SET_8BIT_SIZE;

            /* set clock divider */
            UART1_BAUD_RATE_L = clock_divider;
            UART1_BAUD_RATE_H = (clock_divider >> 8);
            break;
#endif /* UART_1 */
#if UART_2_EN

        case UART_2:
            /* enable RX and TX */
            UART2_RX_TX_EN;
            /* use 8 Bit characters */
            UART2_SET_8BIT_SIZE;

            /* set clock divider */
            UART2_BAUD_RATE_L = clock_divider;
            UART2_BAUD_RATE_H = (clock_divider >> 8);
            break;
#endif /* UART_2 */
#if UART_3_EN

        case UART_3:
            /* enable RX and TX */
            UART3_RX_TX_EN;
            /* use 8 Bit characters */
            UART3_SET_8BIT_SIZE;

            /* set clock divider */
            UART3_BAUD_RATE_L = clock_divider;
            UART3_BAUD_RATE_H = (clock_divider >> 8);
            break;
#endif /* UART_3 */
        default:
            (void)clock_divider;    /* this makes cppcheck happy */
    }

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    switch (uart) {
#if UART_0_EN
        case UART_0:
            for (int i = 0; i < len; i++) {
                while (!UART0_DTREG_EMPTY);
                UART0_DATA_REGISTER = data[i];
            }
            break;
#endif /* UART_0_EN */
#if UART_1_EN
        case UART_1:
            for (int i = 0; i < len; i++) {
                while (!UART1_DTREG_EMPTY);
                UART1_DATA_REGISTER = data[i];
            }
            break;
#endif /* UART_1_EN */
#if UART_2_EN
        case UART_2:
            for (int i = 0; i < len; i++) {
                while (!UART2_DTREG_EMPTY);
                UART2_DATA_REGISTER = data[i];
            }
            break;
#endif /* UART_2_EN */
#if UART_3_EN
        case UART_3:
            for (int i = 0; i < len; i++) {
                while (!UART3_DTREG_EMPTY);
                UART3_DATA_REGISTER = data[i];
            }
            break;
#endif /* UART_3_EN */
    }
}


#if UART_0_EN
ISR(USART0_RX_vect, ISR_BLOCK)
{
    __enter_isr();
    config[UART_0].rx_cb(config[UART_0].arg, UART0_DATA_REGISTER);

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}
#endif /* UART_0_EN */

#if UART_1_EN
ISR(USART1_RX_vect, ISR_BLOCK)
{
    __enter_isr();
    config[UART_1].rx_cb(config[UART_1].arg, UART0_DATA_REGISTER);

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}
#endif /* UART_1_EN */

#if UART_1_EN
ISR(USART2_RX_vect, ISR_BLOCK)
{
    __enter_isr();
    config[UART_2].rx_cb(config[UART_2].arg, UART0_DATA_REGISTER);

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}
#endif /* UART_2_EN */

#if UART_2_EN
ISR(USART2_RX_vect, ISR_BLOCK)
{
    __enter_isr();
    config[UART_3].rx_cb(config[UART_3].arg, UART0_DATA_REGISTER);

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}
#endif /* UART_3_EN */
