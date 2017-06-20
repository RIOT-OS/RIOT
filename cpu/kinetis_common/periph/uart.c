/*
 * Copyright (C) 2017 Eistec AB
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
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
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

static inline void kinetis_set_brfa(UART_Type *dev, uint32_t baudrate, uint32_t clk)
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
    assert(uart < UART_NUMOF);
    /* do basic initialization */
    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return res;
    }
    UART_Type *dev = uart_config[uart].dev;

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    /* enable receive interrupt */
    NVIC_EnableIRQ(uart_config[uart].irqn);
    dev->C2 |= (1 << UART_C2_RIE_SHIFT);

    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    UART_Type *dev = uart_config[uart].dev;
    uint32_t clk;
    uint16_t ubd;

    clk = uart_config[uart].freq;

    /* initialize pins */
    if (uart_config[uart].pin_rx != GPIO_UNDEF) {
        gpio_init_port(uart_config[uart].pin_rx, uart_config[uart].pcr_rx);
    }
    if (uart_config[uart].pin_tx != GPIO_UNDEF) {
        gpio_init_port(uart_config[uart].pin_tx, uart_config[uart].pcr_tx);
    }

    /* Turn on module clock gate */
    *(uart_config[uart].clken) = 1;

    /* disable transmitter and receiver */
    dev->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    /* set defaults, 8-bit mode, no parity */
    dev->C1 = uart_config[uart].mode;

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
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    UART_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        while (!(dev->S1 & UART_S1_TDRE_MASK)) {}
        dev->D = data[i];
    }
}

static inline void irq_handler(uart_t uart)
{
    UART_Type *dev = uart_config[uart].dev;

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

        if (config[uart].rx_cb != NULL) {
            config[uart].rx_cb(config[uart].arg, data);
        }
    }

#if (KINETIS_UART_ADVANCED == 0)
    /* clear overrun flag */
    if (dev->S1 & UART_S1_OR_MASK) {
        dev->S1 = UART_S1_OR_MASK;
    }
#endif

    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(UART_DEV(0));
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(UART_DEV(1));
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(UART_DEV(2));
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(UART_DEV(3));
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(UART_DEV(4));
}
#endif
