/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_uart
 *
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"

#ifndef KINETIS_UART_ADVANCED
/**
 * Attempts to determine the type of the UART,
 * using the BRFA field in the UART C4 register.
 */
#ifdef UART_C4_BRFA
#define KINETIS_UART_ADVANCED    1
#endif
#endif

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
static inline void irq_handler(uart_t uartnum, KINETIS_UART *uart);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t config[UART_NUMOF];

static inline void kinetis_set_brfa(KINETIS_UART *dev, uint32_t baudrate, uint32_t clk)
{
#if KINETIS_UART_ADVANCED
    /* set baudrate fine adjust (brfa) */
    uint8_t brfa = ((((4 * clk) / baudrate) + 1) / 2) % 32;
    dev->C4 = UART_C4_BRFA(brfa);
#endif
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);

    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    /* enable receive interrupt */
    switch (uart) {
#if UART_0_EN

        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            UART_0_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            UART_1_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            break;
#endif

        default:
            return -2;
            break;
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    KINETIS_UART *dev;
    PORT_Type *port;
    uint32_t clk;
    uint16_t ubd;
    uint8_t tx_pin = 0;
    uint8_t rx_pin = 0;
    uint8_t af;

    switch (uart) {
#if UART_0_EN

        case UART_0:
            dev = UART_0_DEV;
            port = UART_0_PORT;
            clk = UART_0_CLK;
            tx_pin = UART_0_TX_PIN;
            rx_pin = UART_0_RX_PIN;
            af = UART_0_AF;
            UART_0_PORT_CLKEN();
            UART_0_CLKEN();
            break;
#endif
#if UART_1_EN

        case UART_1:
            dev = UART_1_DEV;
            port = UART_1_PORT;
            clk = UART_1_CLK;
            tx_pin = UART_1_TX_PIN;
            rx_pin = UART_1_RX_PIN;
            af = UART_1_AF;
            UART_1_PORT_CLKEN();
            UART_1_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    /* configure RX and TX pins, set pin to use alternative function mode */
    port->PCR[rx_pin] = PORT_PCR_MUX(af);
    port->PCR[tx_pin] = PORT_PCR_MUX(af);

    /* disable transmitter and receiver */
    dev->C2 &= ~(1 << UART_C2_TE_SHIFT | 1 << UART_C2_RE_SHIFT);
    /* set defaults, 8-bit mode, no parity */
    dev->C1 = 0;

    /* calculate baudrate */
    ubd = (uint16_t)(clk / (baudrate * 16));

    /* set baudrate */
    dev->BDH = (uint8_t)UART_BDH_SBR(ubd >> 8);
    dev->BDL = (uint8_t)UART_BDL_SBR(ubd);
    kinetis_set_brfa(dev, baudrate, clk);

    /* enable transmitter and receiver */
    dev->C2 |= (1 << UART_C2_TE_SHIFT | 1 << UART_C2_RE_SHIFT);
    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            UART_0_DEV->C2 |= (1 << UART_C2_TIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            UART_1_DEV->C2 |= (1 << UART_C2_TIE_SHIFT);
            break;
#endif

        default:
            break;
    }
}

void uart_tx_end(uart_t uart)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            UART_0_DEV->C2 &= ~(1 << UART_C2_TIE_SHIFT);
            break;
#endif
#if UART_1_EN

        case UART_1:
            UART_1_DEV->C2 &= ~(1 << UART_C2_TIE_SHIFT);
            break;
#endif

        default:
            break;
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            if (UART_0_DEV->S1 & UART_S1_TDRE_MASK) {
                UART_0_DEV->D = (uint8_t)data;
            }

            break;
#endif
#if UART_1_EN

        case UART_1:
            if (UART_1_DEV->S1 & UART_S1_TDRE_MASK) {
                UART_1_DEV->D = (uint8_t)data;
            }

            break;
#endif

        default:
            return -2;
            break;
    }

    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            while (!(UART_0_DEV->S1 & UART_S1_RDRF_MASK));

            *data = (char)UART_0_DEV->D;
            break;
#endif
#if UART_1_EN

        case UART_1:
            while (!(UART_1_DEV->S1 & UART_S1_RDRF_MASK));

            *data = (char)UART_1_DEV->D;
            break;
#endif

        default:
            return -2;
            break;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
#if UART_0_EN

        case UART_0:
            while (!(UART_0_DEV->S1 & UART_S1_TDRE_MASK));

            UART_0_DEV->D = (uint8_t)data;
            break;
#endif
#if UART_1_EN

        case UART_1:
            while (!(UART_1_DEV->S1 & UART_S1_TDRE_MASK));

            UART_1_DEV->D = (uint8_t)data;
            break;
#endif

        default:
            return -2;
            break;
    }

    return 1;
}

#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, UART_0_DEV);
}
#endif

#if UART_1_EN
void UART_1_ISR(void)
{
    irq_handler(UART_1, UART_1_DEV);
}
#endif

static inline void irq_handler(uart_t uartnum, KINETIS_UART *dev)
{
    /*
    * On Cortex-M0, it happens that S1 is read with LDR
    * instruction instead of LDRB. This will read the data register
    * at the same time and arrived byte will be lost. Maybe it's a GCC bug.
    *
    * Observed with: arm-none-eabi-gcc (4.8.3-8+..)
    * It does not happen with: arm-none-eabi-gcc (4.8.3-9+11)
    */

    if (dev->S1 & UART_S1_RDRF_MASK) {
        /* RDRF flag will be cleared when dev-D was read */
        uint8_t data = dev->D;

        if (config[uartnum].rx_cb != NULL) {
            config[uartnum].rx_cb(config[uartnum].arg, data);
        }
    }

    if (dev->S1 & UART_S1_TDRE_MASK) {
        if (config[uartnum].tx_cb == NULL) {
            dev->C2 &= ~(UART_C2_TIE_MASK);
        }
        else if (config[uartnum].tx_cb(config[uartnum].arg) == 0) {
            dev->C2 &= ~(UART_C2_TIE_MASK);
        }
    }

#if (KINETIS_UART_ADVANCED == 0)
    /* clear overrun flag */
    if (dev->S1 & UART_S1_OR_MASK) {
        dev->S1 = UART_S1_OR_MASK;
    }
#endif

    if (sched_context_switch_request) {
        thread_yield();
    }

}
