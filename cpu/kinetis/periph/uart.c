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
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_uart
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

#include <assert.h>

#include "cpu.h"
#include "bit.h"
#include "periph_conf.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef KINETIS_HAVE_LPUART
#ifdef LPUART0
#define KINETIS_HAVE_LPUART     1
#else
#define KINETIS_HAVE_LPUART     0
#endif
#endif /* KINETIS_HAVE_LPUART */

#ifndef KINETIS_HAVE_UART
#ifdef UART0
#define KINETIS_HAVE_UART       1
#else
#define KINETIS_HAVE_UART       0
#endif
#endif /* KINETIS_HAVE_LPUART */

#ifndef KINETIS_UART_ADVANCED
/**
 * Attempts to determine the type of the UART,
 * using the BRFA field in the UART C4 register.
 */
#ifdef UART_C4_BRFA
#define KINETIS_UART_ADVANCED   1
#endif
#endif

#ifndef LPUART_OVERSAMPLING_RATE_MIN
/* Use 10x oversampling at minimum, this will be iterated to achieve a better
 * baud rate match than otherwise possible with a fixed oversampling rate */
/* Hardware reset default value is an oversampling rate of 16 */
#define LPUART_OVERSAMPLING_RATE_MIN (10)
#endif
#ifndef LPUART_OVERSAMPLING_RATE_MAX
#define LPUART_OVERSAMPLING_RATE_MAX (32)
#endif

/* Default LPUART clock setting to avoid compilation failures, define this in
 * periph_conf.h to set board specific configuration if using the LPUART. */
#ifndef LPUART_0_SRC
#define LPUART_0_SRC            0
#endif
#ifndef LPUART_1_SRC
#define LPUART_1_SRC            0
#endif

/**
 * @brief Runtime configuration space, holds pointers to callback functions for RX
 */
static uart_isr_ctx_t config[UART_NUMOF];

static inline void uart_init_pins(uart_t uart);

#if KINETIS_HAVE_UART
static inline void uart_init_uart(uart_t uart, uint32_t baudrate);
#endif
#if KINETIS_HAVE_LPUART
static inline void uart_init_lpuart(uart_t uart, uint32_t baudrate);
#endif

/* Only use the dispatch function for uart_write if both UART and LPUART are
 * available at the same time */
#if KINETIS_HAVE_UART && KINETIS_HAVE_LPUART
#define KINETIS_UART_WRITE_INLINE static inline
KINETIS_UART_WRITE_INLINE void uart_write_uart(uart_t uart, const uint8_t *data, size_t len);
KINETIS_UART_WRITE_INLINE void uart_write_lpuart(uart_t uart, const uint8_t *data, size_t len);
#else
#define KINETIS_UART_WRITE_INLINE
#if KINETIS_HAVE_UART
#define uart_write_uart uart_write
#elif KINETIS_HAVE_LPUART
#define uart_write_lpuart uart_write
#endif
#endif

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    uart_init_pins(uart);

    /* Turn on module clock gate */
    bit_set32(uart_config[uart].scgc_addr, uart_config[uart].scgc_bit);

    switch (uart_config[uart].type) {
#if KINETIS_HAVE_UART
        case KINETIS_UART:
            uart_init_uart(uart, baudrate);
            break;
#endif
#if KINETIS_HAVE_LPUART
        case KINETIS_LPUART:
            uart_init_lpuart(uart, baudrate);
            break;
#endif
        default:
            return UART_NODEV;
    }
    return UART_OK;
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    /* not implemented (yet) */
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    /* not implemented (yet) */
}

#if KINETIS_HAVE_UART && KINETIS_HAVE_LPUART
/* Dispatch function to pass to the proper write function depending on UART type
 * This function is only used when the CPU supports both UART and LPUART. */
void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    switch (uart_config[uart].type) {
        case KINETIS_UART:
            uart_write_uart(uart, data, len);
            break;
        case KINETIS_LPUART:
            uart_write_lpuart(uart, data, len);
            break;
        default:
            return;
    }
}
#endif

static inline void uart_init_pins(uart_t uart)
{
    /* initialize pins */
    if (uart_config[uart].pin_rx != GPIO_UNDEF) {
        gpio_init_port(uart_config[uart].pin_rx, uart_config[uart].pcr_rx);
    }
    if (uart_config[uart].pin_tx != GPIO_UNDEF) {
        gpio_init_port(uart_config[uart].pin_tx, uart_config[uart].pcr_tx);
    }
}

#if KINETIS_HAVE_UART
static inline void uart_init_uart(uart_t uart, uint32_t baudrate)
{
    /* do basic initialization */
    UART_Type *dev = uart_config[uart].dev;

    uint32_t clk;
    uint16_t ubd;

    clk = uart_config[uart].freq;

    /* disable transmitter and receiver */
    dev->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    /* Select mode */
    dev->C1 = uart_config[uart].mode;

    /* calculate baudrate */
    ubd = (uint16_t)(clk / (baudrate * 16));

    /* set baudrate */
    dev->BDH = (uint8_t)UART_BDH_SBR(ubd >> 8);
    dev->BDL = (uint8_t)UART_BDL_SBR(ubd);

#if KINETIS_UART_ADVANCED
    /* set baudrate fine adjust (brfa) */
    uint8_t brfa = ((((4 * clk) / baudrate) + 1) / 2) % 32;
    dev->C4 = UART_C4_BRFA(brfa);

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
#endif /* KINETIS_UART_ADVANCED */

    /* enable transmitter and receiver + RX interrupt */
    dev->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;

    /* enable receive interrupt */
    NVIC_EnableIRQ(uart_config[uart].irqn);
}
#endif /* KINETIS_HAVE_UART */

#if KINETIS_HAVE_UART
KINETIS_UART_WRITE_INLINE void uart_write_uart(uart_t uart, const uint8_t *data, size_t len)
{
    UART_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        while (!(dev->S1 & UART_S1_TDRE_MASK)) {}
        dev->D = data[i];
    }
}

#if defined(UART_0_ISR) || defined(UART_1_ISR) || defined(UART_2_ISR) || \
    defined(UART_3_ISR) || defined(UART_4_ISR)
static inline void irq_handler_uart(uart_t uart)
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
#endif

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler_uart(UART_DEV(0));
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler_uart(UART_DEV(1));
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler_uart(UART_DEV(2));
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler_uart(UART_DEV(3));
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler_uart(UART_DEV(4));
}
#endif

#endif /* KINETIS_HAVE_UART */

#if KINETIS_HAVE_LPUART
static inline void uart_init_lpuart(uart_t uart, uint32_t baudrate)
{
    LPUART_Type *dev = uart_config[uart].dev;

    /* Set LPUART clock source */
#ifdef SIM_SOPT2_LPUART0SRC
    if (dev == LPUART0) {
        SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_LPUART0SRC_MASK) |
            SIM_SOPT2_LPUART0SRC(LPUART_0_SRC);
    }
#endif
#ifdef SIM_SOPT2_LPUART1SRC
    if (dev == LPUART1) {
        SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_LPUART1SRC_MASK) |
            SIM_SOPT2_LPUART1SRC(LPUART_1_SRC);
    }
#endif

    /* Select mode */
    /* transmitter and receiver disabled */
    dev->CTRL = uart_config[uart].mode;

    /* calculate baud rate divisor */
    uint32_t clk = uart_config[uart].freq;
    uint32_t best_err = baudrate;
    uint32_t best_osr = LPUART_OVERSAMPLING_RATE_MIN;
    /* Use the oversampling rate as a baud rate fine adjust tool */
    for (uint32_t osr = LPUART_OVERSAMPLING_RATE_MIN; osr <= LPUART_OVERSAMPLING_RATE_MAX; ++osr) {
        uint32_t div = clk / (osr * baudrate);
        uint32_t actual_baud = clk / (osr * div);
        uint32_t err = ((actual_baud > baudrate) ? actual_baud - baudrate : baudrate - actual_baud);
        if (err < best_err) {
            best_err = err;
            best_osr = osr;
        }
    }

    uint32_t sbr = clk / (best_osr * baudrate);
    /* set baud rate */
    dev->BAUD = LPUART_BAUD_OSR(best_osr - 1) | LPUART_BAUD_SBR(sbr);

    /* enable transmitter and receiver + RX interrupt */
    dev->CTRL |= LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK | LPUART_CTRL_RIE_MASK;

    /* enable receive interrupt */
    NVIC_EnableIRQ(uart_config[uart].irqn);
}

KINETIS_UART_WRITE_INLINE void uart_write_lpuart(uart_t uart, const uint8_t *data, size_t len)
{
    LPUART_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        while ((dev->STAT & LPUART_STAT_TDRE_MASK) == 0) {}
        dev->DATA = data[i];
    }
}

#if defined(LPUART_0_ISR) || defined(LPUART_1_ISR) || defined(LPUART_2_ISR) || \
    defined(LPUART_3_ISR) || defined(LPUART_4_ISR)
static inline void irq_handler_lpuart(uart_t uart)
{
    LPUART_Type *dev = uart_config[uart].dev;
    uint32_t stat = dev->STAT;
    /* Clear all IRQ flags */
    dev->STAT = stat;

    if (stat & LPUART_STAT_RDRF_MASK) {
        /* RDRF flag will be cleared when LPUART_DATA is read */
        uint8_t data = dev->DATA;
        if (stat & (LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK)) {
            if (stat & LPUART_STAT_FE_MASK) {
                DEBUG("LPUART framing error %08" PRIx32 "\n", stat);
            }
            if (stat & LPUART_STAT_PF_MASK) {
                DEBUG("LPUART parity error %08" PRIx32 "\n", stat);
            }
            /* FE is set whenever the next character to be read from LPUART_DATA
             * was received with logic 0 detected where a stop bit was expected. */
            /* PF is set whenever the next character to be read from LPUART_DATA
             * was received when parity is enabled (PE = 1) and the parity bit in
             * the received character does not agree with the expected parity value. */
        }
        /* Only run callback if no error occurred */
        else if (config[uart].rx_cb != NULL) {
            config[uart].rx_cb(config[uart].arg, data);
        }
    }
    if (stat & LPUART_STAT_OR_MASK) {
        /* Input buffer overflow, means that the software was too slow to
         * receive the data */
        DEBUG("LPUART overrun %08" PRIx32 "\n", stat);
    }

    cortexm_isr_end();
}
#endif

#ifdef LPUART_0_ISR
void LPUART_0_ISR(void)
{
    irq_handler_lpuart(UART_DEV(0));
}
#endif

#ifdef LPUART_1_ISR
void LPUART_1_ISR(void)
{
    irq_handler_lpuart(UART_DEV(1));
}
#endif

#ifdef LPUART_2_ISR
void LPUART_2_ISR(void)
{
    irq_handler_lpuart(UART_DEV(2));
}
#endif

#ifdef LPUART_3_ISR
void LPUART_3_ISR(void)
{
    irq_handler_lpuart(UART_DEV(3));
}
#endif

#ifdef LPUART_4_ISR
void LPUART_4_ISR(void)
{
    irq_handler_lpuart(UART_DEV(4));
}
#endif

#endif /* KINETIS_HAVE_LPUART */
