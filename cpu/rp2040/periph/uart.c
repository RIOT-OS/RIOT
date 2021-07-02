/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_rp2040
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file        uart.c
 * @brief       Implementation of the low-level UART driver for the rp2040
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"

static uart_isr_ctx_t uart_isr_ctx[NUM_UART];

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

    if (rx_cb) {
        uart_isr_ctx[uart].arg = arg;
        uart_isr_ctx[uart].rx_cb = rx_cb;
    }

    resets_hw->reset |= RESETS_RESET_IO_BANK0_BITS;
    resets_hw->reset &= ~(RESETS_RESET_IO_BANK0_BITS);
    while (!(resets_hw->reset_done & RESETS_RESET_DONE_IO_BANK0_BITS)) {}

    resets_hw->reset |= RESETS_RESET_PADS_BANK0_BITS;
    resets_hw->reset &= ~(RESETS_RESET_PADS_BANK0_BITS);
    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PADS_BANK0_BITS)) {}

    rx_pin = (uart_config[uart].rx_pin & 0x0000ffff);
    tx_pin = (uart_config[uart].tx_pin & 0x0000ffff);

    padsbank0_hw->io[rx_pin] &= ~(PADS_BANK0_GPIO0_OD_BITS);
    padsbank0_hw->io[rx_pin] |= PADS_BANK0_GPIO0_IE_LSB;
    padsbank0_hw->io[tx_pin] &= ~(PADS_BANK0_GPIO0_OD_BITS);
    padsbank0_hw->io[tx_pin] |= PADS_BANK0_GPIO0_IE_LSB;

    iobank0_hw->io[rx_pin].ctrl =
        (IO_BANK0_GPIO1_CTRL_FUNCSEL_VALUE_UART0_RX << IO_BANK0_GPIO1_CTRL_FUNCSEL_LSB);
    iobank0_hw->io[tx_pin].ctrl =
        (IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_UART0_TX << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB);

    // Baudrate calculation.
    baudrate_div = ((8 * get_clk_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI) * 1000) / baudrate);
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

    // Disable FIFO. Operate in character mode.
    uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_FEN_BITS);

    uart_config[uart].dev->cr |= UART_UARTCR_UARTEN_BITS;

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    size_t wrote = 0;

    while(wrote != len) {
        uart_config[uart].dev->dr = *data;

        while((uart_config[uart].dev->fr & UART_UARTFR_BUSY_BITS)) {}

        data++;

        wrote++;
    }
}

void isr_uart(int dev) {
    (void)(dev);
}

void isr_uart0(void) {
    isr_uart(0);

    cortexm_isr_end();
}
void isr_uart1(void) {
    isr_uart(1);

    cortexm_isr_end();
}
