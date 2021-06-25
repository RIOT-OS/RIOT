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

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t uart_isr_ctx[NUM_UART];

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    uint32_t rx_idx_pin;
    uint32_t tx_idx_pin;

    (void)(baudrate);
    (void)(rx_cb);
    (void)(arg);
    (void)(uart_isr_ctx[0]);

    // Release UART HW reset.


    switch(uart) {
        case 0:
        {
            resets_hw->reset |= RESETS_RESET_UART0_BITS;

            resets_hw->reset &= ~RESETS_RESET_UART0_BITS;

            while (!(resets_hw->reset_done & RESETS_RESET_DONE_UART0_BITS)) {}

            break;
        }

        case 1:
        {
            resets_hw->reset |= RESETS_RESET_UART1_BITS;

            resets_hw->reset &= ~RESETS_RESET_UART1_BITS;

            while (!(resets_hw->reset_done & RESETS_RESET_DONE_UART1_BITS)) {}

            break;
        }

        default:
            return UART_NODEV;
    }

    resets_hw->reset &= ~RESETS_RESET_IO_BANK0_BITS;
    resets_hw->reset &= ~RESETS_RESET_PADS_BANK0_BITS;

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_IO_BANK0_BITS)) {}
    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PADS_BANK0_BITS)) {}

    rx_idx_pin = (uart_config[0].rx_pin & 0x00ff);
    tx_idx_pin = (uart_config[0].tx_pin & 0x00ff);

    iobank0_hw->io[rx_idx_pin].ctrl |=
        (IO_BANK0_GPIO1_CTRL_FUNCSEL_VALUE_UART0_RX << IO_BANK0_GPIO1_CTRL_FUNCSEL_LSB);
    iobank0_hw->io[tx_idx_pin].ctrl |=
        (IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_UART0_TX << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB);

    /*padsbank0_hw->io[rx_idx_pin] |= (1 << PADS_BANK0_GPIO1_OD_LSB);
    padsbank0_hw->io[rx_idx_pin] |= (1 << PADS_BANK0_GPIO1_IE_LSB);

    padsbank0_hw->io[tx_idx_pin] &= ~(1 << PADS_BANK0_GPIO0_OD_LSB);
    padsbank0_hw->io[tx_idx_pin] &= ~(1 << PADS_BANK0_GPIO0_IE_LSB);*/


    /*
        uint32_t baud_rate_div = (8 * clock_get_hz(clk_peri) / baudrate);
        uint32_t baud_ibrd = baud_rate_div >> 7;
        uint32_t baud_fbrd = ((baud_rate_div & 0x7f) + 1) / 2;

        if (baud_ibrd == 0) {
            baud_ibrd = 1;
            baud_fbrd = 0;
        } else if (baud_ibrd >= 65535) {
            baud_ibrd = 65535;
            baud_fbrd = 0;
        }

        // Load PL011's baud divisor registers
        uart_get_hw(uart)->ibrd = baud_ibrd;
        uart_get_hw(uart)->fbrd = baud_fbrd;

        // PL011 needs a (dummy) line control register write to latch in the
        // divisors. We don't want to actually change LCR contents here.
        hw_set_bits(&uart_get_hw(uart)->lcr_h, 0);
    */

    uart_config[uart].dev->ibrd = 78;
    uart_config[uart].dev->fbrd = 8;

    uart_config[uart].dev->lcr_h |= (3 << UART_UARTLCR_H_WLEN_LSB);
    uart_config[uart].dev->lcr_h &= ~(UART_UARTLCR_H_PEN_BITS);

    uart_config[uart].dev->cr |= (UART_UARTCR_TXE_BITS | UART_UARTCR_UARTEN_BITS);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)(uart);
    (void)(data);

    //uint32_t i = 0;
    size_t _len = len;

    while(_len != 0) {
        uart0_hw->dr = 'X';

        while((uart0_hw->fr & UART_UARTFR_BUSY_BITS)) {}

        //i++;
        _len--;
    }
}

/**
 * The UART interrupt handler.
 */

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
