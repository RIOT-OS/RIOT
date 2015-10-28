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
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t config[UART_NUMOF];

static inline void kinetis_set_brfa(KINETIS_UART *dev, uint32_t baudrate, uint32_t clk)
{
#if KINETIS_UART_ADVANCED
    /* set baudrate fine adjust (brfa) */
    uint8_t brfa = ((((4 * clk) / baudrate) + 1) / 2) % 32;
    dev->C4 = UART_C4_BRFA(brfa);
#endif
}

static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* do basic initialization */
    int res = init_base(uart, baudrate);

    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
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

static int init_base(uart_t uart, uint32_t baudrate)
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
    dev->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    /* set defaults, 8-bit mode, no parity */
    dev->C1 = 0;

    /* calculate baudrate */
    ubd = (uint16_t)(clk / (baudrate * 16));

    /* set baudrate */
    dev->BDH = (uint8_t)UART_BDH_SBR(ubd >> 8);
    dev->BDL = (uint8_t)UART_BDL_SBR(ubd);
    kinetis_set_brfa(dev, baudrate, clk);

#if KINETIS_UART_ADVANCED
    /* Enable FIFO buffers */
    dev->PFIFO |= UART_PFIFO_RXFE_MASK | UART_PFIFO_TXFE_MASK;
    /* Set level to trigger TDRE flag whenever there is space in the TXFIFO */
    /* FIFO size is 2^(PFIFO_TXFIFOSIZE + 1) (4, 8, 16 ...) for values != 0.
     * TXFIFOSIZE == 0 means size = 1 (i.e. only one byte, no hardware FIFO) */
    if ((dev->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) != 0) {
        uint8_t txfifo_size =
            (2 << ((dev->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) >>
                    UART_PFIFO_TXFIFOSIZE_SHIFT));
        dev->TWFIFO = UART_TWFIFO_TXWATER(txfifo_size - 1);
    }
    else {
        /* Missing hardware support */
        dev->TWFIFO = 0;
    }
    /* Trigger RX interrupt when there is 1 byte or more in the RXFIFO */
    dev->RWFIFO = 1;
    /* Clear all hardware buffers now, this must be done whenever the FIFO
     * enable flags are modified. */
    dev->CFIFO = UART_CFIFO_RXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK;
#endif

    /* enable transmitter and receiver */
    dev->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    KINETIS_UART *dev;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            dev = UART_0_DEV;
            break;
#endif
#if UART_1_EN
        case UART_1:
            dev = UART_1_DEV;
            break;
#endif
        default:
            return;
    }

    for (size_t i = 0; i < len; i++) {
        while (!(dev->S1 & UART_S1_TDRE_MASK));
        dev->D = data[i];
    }
}

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
