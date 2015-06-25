/*
 * Copyright (C) 2015 Eistec AB
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
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "kinetis_lpm.h"

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

/**
 * @brief 1 character TX queue.
 *
 * Works well enough for now, possible improvements include using the ringbuffer
 * library or DMA transfers.
 */
static atomic_int_t uart_next_tx_char[UART_NUMOF];

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
            UART_0_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            /* Enable receiver edge detect interrupt */
            UART_0_DEV->BDH |= UART_BDH_RXEDGIE_MASK;
            break;
#endif
#if UART_1_EN

        case UART_1:
            UART_1_DEV->C2 |= (1 << UART_C2_RIE_SHIFT);
            /* Enable receiver edge detect interrupt */
            UART_1_DEV->BDH |= UART_BDH_RXEDGIE_MASK;
            break;
#endif

        default:
            return -2;
            break;
    }

    if (config[uart].rx_cb != NULL) {
        /* Disable LLS if we have an RX callback configured since the UART can
         * not wake the CPU from LLS. */
        /* The RXEDGIF interrupt only works in VLPS and higher modes */
        kinetis_lpm_inhibit_lls();
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
            /* Enable IRQs in the NVIC because we are using TX IRQs for blocking
             * transfers as well */
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
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
            /* Enable IRQs in the NVIC because we are using TX IRQs for blocking
             * transfers as well */
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            break;
#endif

        default:
            return -1;
    }

    /* Initialize TX "queue" */
    uart_next_tx_char[uart].value = -1;

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
    UART_Type* dev;
    int qval;

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
            return -2;
            break;
    }

    /* Avoid sign extension when converting from char, regardless of char signedness */
    qval = (unsigned char)data;

    /* Try to write to the TX buffer */
    atomic_cas(&uart_next_tx_char[uart], -1, qval);

    /* Enable TX interrupt, the ISR will push data to the transfer register if
     * the atomic write operation was successful. */
    dev->C2 |= UART_C2_TIE_MASK;

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
    UART_Type* dev;
    int qval;

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
            return -2;
            break;
    }

    /* Avoid sign extension when converting from char, regardless of char signedness */
    qval = (unsigned char)data;
    /* Check if interrupts are enabled */
    uint8_t mask = __get_PRIMASK();

    if (mask == 0) {
        /* Try to write to the TX buffer until we succeed */
        while (!atomic_cas(&uart_next_tx_char[uart], -1, qval)) {
            /* TODO: sleep for a bit */
        }
        /* Enable TX interrupt, the ISR will push data to the transfer register. */
        dev->C2 |= UART_C2_TIE_MASK;
    } else {
        /* Interrupts are disabled, do a busy wait */
        while ((dev->S1 & UART_S1_TDRE_MASK) == 0) {
        }
        dev->D = data;
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

static inline void irq_rx_handler(uart_t uartnum, KINETIS_UART *dev)
{
    /* Flag used for keeping track of when we may sleep */
    static uint8_t receiving[UART_NUMOF] = {0};

    /*
     * On Cortex-M0, it happens that S1 is read with LDR
     * instruction instead of LDRB. This will read the data register
     * at the same time and arrived byte will be lost. Maybe it's a GCC bug.
     *
     * Observed with: arm-none-eabi-gcc (4.8.3-8+..)
     * It does not happen with: arm-none-eabi-gcc (4.8.3-9+11)
     */

    if(dev->S1 & UART_S1_RDRF_MASK) {
        /* RDRF flag is cleared by first reading S1, then reading D */
        volatile uint8_t c = dev->D;
        if (config[uartnum].rx_cb != NULL) {
            config[uartnum].rx_cb(config[uartnum].arg, c);
        }
    }

    if ((dev->S2 & UART_S2_RAF_MASK) == 0) {
        /* Receiver idle */
        if (receiving[uartnum] != 0) {
            receiving[uartnum] = 0;
            kinetis_lpm_uninhibit_stop();
        }
    }

    if((dev->S2 & UART_S2_RXEDGIF_MASK)) {
        /* Woken up by edge detect */
        if (receiving[uartnum] == 0) {
            receiving[uartnum] = 1;
            kinetis_lpm_inhibit_stop();
        }
        /* Clear RX wake-up flag by writing a 1 to it */
        dev->S2 = UART_S2_RXEDGIF_MASK;
    }
}

static inline void irq_tx_handler(uart_t uartnum, KINETIS_UART *dev)
{
    /* Flag used for keeping track of when we may sleep */
    static uint8_t transmitting[UART_NUMOF] = {0};

    if((dev->C2 & UART_C2_TCIE_MASK) && (dev->S1 & UART_S1_TC_MASK)) {
        if (transmitting[uartnum] != 0) {
            /* transmission complete, allow STOP modes again */
            transmitting[uartnum] = 0;
            kinetis_lpm_uninhibit_stop();
        }
        /* Disable transmission complete interrupt */
        dev->C2 &= ~(UART_C2_TCIE_MASK);
    }

    if((dev->C2 & UART_C2_TIE_MASK) && (dev->S1 & UART_S1_TDRE_MASK)) {
        /* Pop the transfer queue */
        int qval = uart_next_tx_char[uartnum].value;
        uart_next_tx_char[uartnum].value = -1;
        if (qval < 0) {
            /* Empty buffer, disable this interrupt */
            dev->C2 &= ~(UART_C2_TIE_MASK);
            /* Enable transmission complete interrupt. */
            dev->C2 |= UART_C2_TCIE_MASK;
        } else {
            /* queue next byte to the hardware FIFO */
            dev->D = (uint8_t)(qval & 0xff);
            if (transmitting[uartnum] == 0) {
                transmitting[uartnum] = 1;
                kinetis_lpm_inhibit_stop();
            }
        }
        /* Run TX callback if it is set */
        if (config[uartnum].tx_cb != NULL) {
            if (config[uartnum].tx_cb(config[uartnum].arg)) {
                /* Enable TX IRQ, disabling is performed when the queue is empty above. */
                dev->C2 |= UART_C2_TIE_MASK;
            }
        }
    }

#if (KINETIS_UART_ADVANCED == 0)
    /* clear overrun flag */
    if (dev->S1 & UART_S1_OR_MASK) {
        dev->S1 = UART_S1_OR_MASK;
    }
#endif

}

static inline void irq_handler(uart_t uartnum, KINETIS_UART *dev)
{
    irq_rx_handler(uartnum, dev);

    irq_tx_handler(uartnum, dev);

    if (sched_context_switch_request) {
        thread_yield();
    }

}
