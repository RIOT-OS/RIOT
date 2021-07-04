/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @ingroup drivers_periph_uart
 * @{
 *
 * @file uart.c
 * @brief Implementation of the low-level UART driver for the rp2040
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"

static uart_isr_ctx_t uart_isr_ctx[UART_NUMOF];

static void isr_uart(uart_t dev);

static void isr_uart(uart_t dev) {
    uint8_t data =
        (uint8_t)(uart_config[UART_DEV(0)].dev->dr & UART_UARTDR_DATA_BITS);

    uart_isr_ctx[dev].rx_cb(uart_isr_ctx[dev].arg, data);
}

int uart_init(
    uart_t uart,
    uint32_t baudrate,
    uart_rx_cb_t rx_cb,
    void *arg
) {
    uint32_t rx_pin;
    uint32_t tx_pin;

    uint32_t baudrate_i;
    uint32_t baudrate_f;
    uint32_t baudrate_div;

    io_rw_32 *ppb_nvic_iser;
    io_rw_32 *ppb_nvic_icpr;

    assert(uart < UART_NUMOF);

    if (uart == UART_DEV(0)) {
        resets_hw->reset |= RESETS_RESET_UART0_BITS;
        resets_hw->reset &= ~(RESETS_RESET_UART0_BITS);

        while (!(resets_hw->reset_done & RESETS_RESET_DONE_UART0_BITS)) {}
    }
    else if (uart == UART_DEV(1)) {
        resets_hw->reset |= RESETS_RESET_UART1_BITS;
        resets_hw->reset &= ~(RESETS_RESET_UART1_BITS);

        while (!(resets_hw->reset_done & RESETS_RESET_DONE_UART1_BITS)) {}
    }
    else {
        return UART_NODEV;
    }

    ppb_nvic_iser =
        (io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET);

    ppb_nvic_icpr =
        (io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET);

    tx_pin = (uart_config[uart].tx_pin & 0x0000ffff);
    rx_pin = (uart_config[uart].rx_pin & 0x0000ffff);

    // Reset IO pad and bank.
    resets_hw->reset |= RESETS_RESET_IO_BANK0_BITS;
    resets_hw->reset &= ~(RESETS_RESET_IO_BANK0_BITS);

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_IO_BANK0_BITS)) {}

    resets_hw->reset |= RESETS_RESET_PADS_BANK0_BITS;
    resets_hw->reset &= ~(RESETS_RESET_PADS_BANK0_BITS);

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PADS_BANK0_BITS)) {}

    /*
     * Enable both input and output at the pads to
     * the UART peripheral can configure accordingly.
     */
    padsbank0_hw->io[tx_pin] &= ~(PADS_BANK0_GPIO0_OD_BITS);
    padsbank0_hw->io[tx_pin] |= PADS_BANK0_GPIO0_IE_BITS;

    padsbank0_hw->io[rx_pin] &= ~(PADS_BANK0_GPIO0_OD_BITS);
    padsbank0_hw->io[rx_pin] |= PADS_BANK0_GPIO0_IE_BITS;

    // Select UART function for the provided pins.
    iobank0_hw->io[tx_pin].ctrl &= ~(IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS);
    iobank0_hw->io[tx_pin].ctrl |=
        (IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_UART0_TX << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB);

    iobank0_hw->io[rx_pin].ctrl &= ~(IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS);
    iobank0_hw->io[rx_pin].ctrl |=
        (IO_BANK0_GPIO1_CTRL_FUNCSEL_VALUE_UART0_RX << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB);

    // Baudrate calculation.
    baudrate_div =
        (get_clk_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI) * 1000 * 8) / baudrate;

    baudrate_i = (baudrate_div >> 7);

    if (baudrate_i == 0) {
        baudrate_i = 1;
        baudrate_f = 0;
    } else if (baudrate_i >= 65535) {
        baudrate_i = 65535;
        baudrate_f = 0;
    }  else {
        baudrate_f = ((baudrate_div & 0x7f) + 1) / 2;
    }

    uart_config[uart].dev->ibrd = baudrate_i;
    uart_config[uart].dev->fbrd = baudrate_f;

    // UART format configuration.
    uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_WLEN_BITS);
    uart_config[uart].dev->lcr_h |=
        ((uart_config[uart].data_bits - 5) << UART_UARTLCR_H_WLEN_LSB);

    if(uart_config[uart].stop_bits == UART_STOP_BITS_1) {
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_STP2_BITS);
    }
    else if (uart_config[uart].stop_bits == UART_STOP_BITS_2) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_STP2_BITS;
    }

    if(uart_config[uart].parity == UART_PARITY_NONE) {
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_PEN_BITS);
    }
    else if (uart_config[uart].parity == UART_PARITY_ODD) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_PEN_BITS;
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_EPS_BITS);
    }
    else if (uart_config[uart].parity == UART_PARITY_EVEN) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_PEN_BITS;
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_EPS_BITS;
    }

    // Disable FIFO, operate in character mode.
    uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_FEN_BITS);

    // Setup interrupt driven RX only if receive callback is provided.
    if (rx_cb) {
        uart_isr_ctx[uart].arg = arg;
        uart_isr_ctx[uart].rx_cb = rx_cb;

        // Enable corresponding CPU interrupts.
        if (uart == UART_DEV(0)) {
            *ppb_nvic_icpr |= (1 << 20);
            *ppb_nvic_iser |= (1 << 20);
        }
        else if (uart == UART_DEV(1)) {
            *ppb_nvic_icpr |= (1 << 21);
            *ppb_nvic_iser |= (1 << 21);
        }

        // Enable RX interrupt.
        uart_config[uart].dev->imsc |= UART_UARTIMSC_RXIM_BITS;
    }

    // Start UART operation.
    uart_config[uart].dev->cr |=
        (UART_UARTCR_UARTEN_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_RXE_BITS);

    return UART_OK;
}

int uart_mode(
    uart_t uart,
    uart_data_bits_t data_bits,
    uart_parity_t parity,
    uart_stop_bits_t stop_bits
) {
    assert(uart < UART_NUMOF);

    if (((data_bits - 5) < 0) || ((data_bits - 5) > 3)) {
        return UART_NOMODE;
    }

    uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_WLEN_BITS);
    uart_config[uart].dev->lcr_h |=
        ((data_bits - 5) << UART_UARTLCR_H_WLEN_LSB);

    if(stop_bits == UART_STOP_BITS_1) {
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_STP2_BITS);
    }
    else if (stop_bits == UART_STOP_BITS_2) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_STP2_BITS;
    }
    else {
        return UART_NOMODE;
    }

    if(parity == UART_PARITY_NONE) {
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_PEN_BITS);
    }
    else if (parity == UART_PARITY_ODD) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_PEN_BITS;
        uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_EPS_BITS);
    }
    else if (parity == UART_PARITY_EVEN) {
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_PEN_BITS;
        uart_config[uart].dev->lcr_h |= UART_UARTLCR_H_EPS_BITS;
    }
    else {
        return UART_NOMODE;
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    size_t wrote = 0;

    while(wrote < len) {
        uart_config[uart].dev->dr = *data;

        while((uart_config[uart].dev->fr & UART_UARTFR_BUSY_BITS)) {}

        data++;
        wrote++;
    }
}

void uart_poweron(uart_t uart) {
    assert(uart < UART_NUMOF);

    resets_hw->reset &= ~(RESETS_RESET_UART0_BITS);

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_UART0_BITS)) {}
}

void uart_poweroff(uart_t uart) {
    assert(uart < UART_NUMOF);

    resets_hw->reset |= RESETS_RESET_UART0_BITS;
}

// Interrupt handlers.

void isr_uart0(void) {
    isr_uart(UART_DEV(0));

    // Clear RX interrupt.
    uart_config[UART_DEV(0)].dev->icr &= ~(UART_UARTICR_RXIC_BITS);

    cortexm_isr_end();
}

void isr_uart1(void) {
    isr_uart(UART_DEV(1));

    // Clear RX interrupt.
    uart_config[UART_DEV(1)].dev->icr &= ~(UART_UARTICR_RXIC_BITS);

    cortexm_isr_end();
}
