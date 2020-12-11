/*
 * Copyright (C) 2014 Loci Controls Inc.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <assert.h>
#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* Pin functions and interrupt definitions for the two UARTs */
#define UART_RXD(X)         (cc2538_ioc_pin_t)(2 * (X))
#define UART_TXD(X)         (cc2538_ioc_sel_t)(2 * (X))
#define UART_IRQ(X)         (IRQn_Type)(5 + (X))

/* Bit field definitions for the UART Line Control Register: */
#define FEN                 (1 << 4)    /**< Enable FIFOs */

/* Bit masks for the UART Masked Interrupt Status (MIS) Register: */
#define OEMIS               (1 << 10)   /**< UART overrun errors */
#define BEMIS               (1 << 9)    /**< UART break error */
#define FEMIS               (1 << 7)    /**< UART framing error */
#define RTMIS               (1 << 6)    /**< UART RX time-out */
#define TXMIS               (1 << 5)    /**< UART TX masked interrupt */
#define RXMIS               (1 << 4)    /**< UART RX masked interrupt */

#define UART_CTL_HSE_VALUE  (0)
#define DIVFRAC_NUM_BITS    (6)
#define DIVFRAC_MASK        ((1 << DIVFRAC_NUM_BITS) - 1)

enum {
    FIFO_LEVEL_1_8TH = 0,
    FIFO_LEVEL_2_8TH = 1,
    FIFO_LEVEL_4_8TH = 2,
    FIFO_LEVEL_6_8TH = 3,
    FIFO_LEVEL_7_8TH = 4,
};

/* Valid word lengths for the LCRHbits.WLEN bit field: */
enum {
    WLEN_5_BITS = 0,
    WLEN_6_BITS = 1,
    WLEN_7_BITS = 2,
    WLEN_8_BITS = 3,
};

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

#ifdef MODULE_PERIPH_UART_NONBLOCKING

#include "tsrb.h"
/**
 * @brief   Allocate for tx ring buffers
 */
static tsrb_t uart_tx_rb[UART_NUMOF];
static uint8_t uart_tx_rb_buf[UART_NUMOF][UART_TXBUF_SIZE];
#endif

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    cc2538_uart_t *u = uart_config[uart].dev;

    /* uart_num refers to the CPU UART peripheral number, which may be
     * different from the value of the uart variable, depending on the board
     * configuration.
     */
    unsigned int uart_num = ((uintptr_t)u - (uintptr_t)UART0_BASEADDR) / 0x1000;

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    /* set up the TX buffer */
    tsrb_init(&uart_tx_rb[uart], uart_tx_rb_buf[uart], UART_TXBUF_SIZE);
#endif

    /* Configure the Rx and Tx pins. If no callback function is defined,
     * the UART should be initialised in Tx only mode.
     */
    if (rx_cb) {
        gpio_init_af(uart_config[uart].rx_pin, UART_RXD(uart_num), GPIO_IN);
    }
    gpio_init_af(uart_config[uart].tx_pin, UART_TXD(uart_num), GPIO_OUT);

    /* Enable clock for the UART while Running, in Sleep and Deep Sleep */
    SYS_CTRL_RCGCUART |= (1 << uart_num);
    SYS_CTRL_SCGCUART |= (1 << uart_num);
    SYS_CTRL_DCGCUART |= (1 << uart_num);

    /* Make sure the UART is disabled before trying to configure it */
    u->cc2538_uart_ctl.CTL = 0;

    /* Run on SYS_DIV */
    u->CC = 0;

    /* On the CC2538, hardware flow control is supported only on UART1 */
#ifdef MODULE_PERIPH_UART_HW_FC
    if (uart_config[uart].rts_pin != GPIO_UNDEF) {
        assert(u != UART0_BASEADDR);
        gpio_init_af(uart_config[uart].rts_pin, UART1_RTS, GPIO_OUT);
        u->cc2538_uart_ctl.CTLbits.RTSEN = 1;
    }

    if (uart_config[uart].cts_pin != GPIO_UNDEF) {
        assert(u != UART0_BASEADDR);
        gpio_init_af(uart_config[uart].cts_pin, UART1_CTS, GPIO_IN);
        u->cc2538_uart_ctl.CTLbits.CTSEN = 1;
    }
#endif

    /*
     * UART Interrupt Setup:
     * Acknowledge Overrun, Break and Framing Errors
     * Acknowledge RX Timeout and Rx
     */
    u->cc2538_uart_im.IM = (OEMIS | BEMIS | FEMIS | RTMIS | RXMIS);

    /* Set FIFO interrupt levels and enable Rx and/or Tx: */
    if (rx_cb) {
        u->cc2538_uart_ifls.IFLSbits.RXIFLSEL = FIFO_LEVEL_4_8TH; /**< MCU default */
        u->cc2538_uart_ctl.CTLbits.RXE = 1;
    }
    u->cc2538_uart_ifls.IFLSbits.TXIFLSEL = FIFO_LEVEL_4_8TH;     /**< MCU default */
    u->cc2538_uart_ctl.CTLbits.TXE = 1;

    /* Enable high speed (UART is clocked using system clock divided by 8
     * rather than 16)
     */
    u->cc2538_uart_ctl.CTLbits.HSE = UART_CTL_HSE_VALUE;

    /* Set the divisor for the baud rate generator */
    uint32_t divisor = sys_clock_freq();
    divisor <<= UART_CTL_HSE_VALUE + 2;
    divisor += baudrate / 2; /**< Avoid a rounding error */
    divisor /= baudrate;
    u->IBRD = divisor >> DIVFRAC_NUM_BITS;
    u->FBRD = divisor & DIVFRAC_MASK;

    /* Configure line control for 8-bit, no parity, 1 stop bit and enable FIFO */
    u->cc2538_uart_lcrh.LCRH = (WLEN_8_BITS << 5) | FEN;

    /* register callbacks and enable UART irq */
    if (rx_cb) {
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
    }

    if (IS_USED(MODULE_PERIPH_UART_NONBLOCKING) || rx_cb) {
        NVIC_EnableIRQ(UART_IRQ(uart_num));
    }

    /* UART Enable */
    u->cc2538_uart_ctl.CTLbits.UARTEN = 1;

    return UART_OK;
}

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assert(uart < UART_NUMOF);

    assert(data_bits == UART_DATA_BITS_5 ||
           data_bits == UART_DATA_BITS_6 ||
           data_bits == UART_DATA_BITS_7 ||
           data_bits == UART_DATA_BITS_8);

    assert(parity == UART_PARITY_NONE ||
           parity == UART_PARITY_EVEN ||
           parity == UART_PARITY_ODD ||
           parity == UART_PARITY_MARK ||
           parity == UART_PARITY_SPACE);

    assert(stop_bits == UART_STOP_BITS_1 ||
           stop_bits == UART_STOP_BITS_2);

    cc2538_reg_t *lcrh = &(uart_config[uart].dev->cc2538_uart_lcrh.LCRH);
    uint32_t tmp = *lcrh;
    tmp &= ~(UART_LCRH_WLEN_M | UART_LCRH_FEN_M | UART_LCRH_STP2_M |
             UART_LCRH_PEN | UART_LCRH_EPS | UART_LCRH_SPS);
    *lcrh = tmp | data_bits | parity | stop_bits;
    return 0;
}
#endif

static inline void send_byte(uart_t uart, uint8_t byte)
{
    /* Block if the TX FIFO is full */
    while (uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFF) {}
    uart_config[uart].dev->DR = byte;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);
#ifdef MODULE_PERIPH_UART_NONBLOCKING
    for (size_t i = 0; i < len; i++) {
        uart_config[uart].dev->cc2538_uart_im.IM |= TXMIS;
        if (irq_is_in() || __get_PRIMASK()) {
            /* if ring buffer is full free up a spot */
            if (tsrb_full(&uart_tx_rb[uart])) {
                send_byte(uart, tsrb_get_one(&uart_tx_rb[uart]));
            }
            /* if FIFO is full write to the buffer */
            if (uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFF) {
                tsrb_add_one(&uart_tx_rb[uart], data[i]);
            }
            /* if tx FIFO is not full then add data to the FIFO */
            else {
                /* if there is already data in the tsrb buffer write that byte */
                int byte = tsrb_get_one(&uart_tx_rb[uart]);
                if (byte >= 0) {
                    uart_config[uart].dev->DR = byte;
                    tsrb_add_one(&uart_tx_rb[uart], data[i]);
                }
                /* If there is not data in the buffer directly write the
                   current byte*/
                else {
                    uart_config[uart].dev->DR = data[i];
                }
            }
        }
        else {
            /* The Tx FIFO will only triggered an interrupt when it gets
               below TXIFLSEL. If there is no data in the FIFO the ISR
               will never trigger, so always write directly to the FIFO
               if its not full to make sure the process is always
               bootstrapped */
            /* If Tx FIFO is full then add to ring buffer */
            if (uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFF) {
                while (tsrb_add_one(&uart_tx_rb[uart], data[i]) < 0) {}
            }
            /* If tx FIFO is not full then add data to the FIFO */
            else {
                /* if there is already data in the tsrb buffer write that byte */
                /* need to disable IRQs in case FIFO gets empty enough
                   to trigger an ISR */
                unsigned state = irq_disable();
                int byte = tsrb_get_one(&uart_tx_rb[uart]);
                if (byte >= 0) {
                    uart_config[uart].dev->DR = byte;
                    irq_restore(state);
                    while (tsrb_add_one(&uart_tx_rb[uart], data[i]) < 0) {}
                }
                /* If there is not data in the buffer directly write the
                   current byte */
                else {
                    irq_restore(state);
                    uart_config[uart].dev->DR = data[i];
                }
            }
        }
    }
#else
    for (size_t i = 0; i < len; i++) {
        send_byte(uart, data[i]);
    }
    /* Wait for the TX FIFO to clear */
    while (!uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFE) {}
#endif
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    /* Turn the clock on first, in case it has been turned off */
    SYS_CTRL->cc2538_sys_ctrl_unnamed1.RCGCUART |= (1 << uart);

    uart_config[uart].dev->cc2538_uart_ctl.CTLbits.UARTEN = 1;
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    /* Wait for the TX FIFO to clear */
    while (uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFF) {}

    uart_config[uart].dev->cc2538_uart_ctl.CTLbits.UARTEN = 0;

    /* Turn the clock off afterwards to save power */
    SYS_CTRL->cc2538_sys_ctrl_unnamed1.RCGCUART &= ~(1 << uart);
}

#ifdef MODULE_PERIPH_UART_NONBLOCKING
static inline void irq_handler_tx(uart_t uart)
{
    /* fill up FIFO again */
    while (!uart_config[uart].dev->cc2538_uart_fr.FRbits.TXFF) {
        int byte = tsrb_get_one(&uart_tx_rb[uart]);
        if (byte >= 0) {
            uart_config[uart].dev->DR = byte;
        }
        else {
            /* disable the interrupt if there are no more bytes to send */
            uart_config[uart].dev->cc2538_uart_im.IM &= ~TXMIS;
            break;
        }
    }
}
#endif

static inline void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);

    cc2538_uart_t *u = uart_config[uart].dev;

    /* Latch the Masked Interrupt Status and clear any active flags */
    uint16_t mis = uart_config[uart].dev->cc2538_uart_mis.MIS;
    uart_config[uart].dev->ICR = mis;

    while (uart_config[uart].dev->cc2538_uart_fr.FRbits.RXFE == 0) {
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, uart_config[uart].dev->DR);
    }

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    if (mis & TXMIS) {
        irq_handler_tx(uart);
    }
#endif

    if (mis & (OEMIS | BEMIS | FEMIS)) {
        /* Clear error status */
        u->cc2538_uart_dr.ECR = 0xFF;
    }

    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler((uart_t)0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler((uart_t)1);
}
#endif
