/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       GBA periph/uart driver
 *
 * @author      Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 *
 * @}
 */

#include "periph/uart.h"
#include "periph_gba.h"

static gba_uart_t uart_settings;
static uart_rx_cb_t uart_cb = NULL;
static void *uart_cb_arg = NULL;

void gba_uart_isr(void)
{
    if (uart_cb) {
        while (!GBA_UART->receive_data_empty) {
            uart_cb(uart_cb_arg, GBA_SIO_DATA8);
        }
    }

    if (GBA_UART->send_data_full) {
        /* ToDo: No-op */
    }

    if (GBA_UART->error) {
        /* ToDo: Handle Error */
    }
}

static inline int _baud_to_flag(uint32_t baud)
{
    switch (baud) {
    case 9600:
        return 0;
    case 38400:
        return 1;
    case 57600:
        return 2;
    case 115200:
        return 3;
    default:
        return UART_NOBAUD;
    }
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    (void)uart;
    int baud_flag = _baud_to_flag(baud);
    if (baud_flag == UART_NOBAUD) {
        return baud_flag;
    }

    uart_settings.baud = baud_flag;
    uart_settings.cts = 0;       /* disable / always send */
    uart_settings.parity = 0;    /* even */
    uart_settings.data_bits = 1; /* 8 bits */
    uart_settings.fifo_enable = 1;
    uart_settings.parity_enable = 0;
    uart_settings.send_enable = 1;
    uart_settings.receive_enable = 1;
    uart_settings.must_be_1_for_uart = 3;
    uart_settings.irq_enable = 1;

    /* apply settings */
    *GBA_UART = uart_settings;

    uart_cb = rx_cb;
    uart_cb_arg = arg;

    return 0;
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    (void)uart;

    if ((parity == UART_PARITY_MARK) || (parity == UART_PARITY_SPACE)) {
        return -ENOTSUP;
    }

    if ((data_bits == UART_DATA_BITS_5) || (data_bits == UART_DATA_BITS_6)) {
        return -ENOTSUP;
    }

    /* ToDo: Check hardware what the GBA uses */
    if (stop_bits == UART_STOP_BITS_1) {
        return -ENOTSUP;
    }

    uart_settings.parity = parity == UART_PARITY_ODD;
    uart_settings.data_bits = data_bits == UART_DATA_BITS_8;
    /* apply settings */
    *GBA_UART = uart_settings;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void)uart;

    int i = 0;
    while (len > 0) {
        /* wait for hardware to get ready */
        while (GBA_UART->send_data_full) {}
        GBA_SIO_DATA8 = data[i++];
        len--;
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    /* re-apply settings */
    *GBA_UART = uart_settings;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    /* The uart can not really be turned off, so we just disable sending & receiving
     * In the future, we might release the SIO port as a whole but currently there's
     * no point in doing that as we don't have drivers for the alternate modes */
    GBA_UART->send_enable = 0;
    GBA_UART->receive_enable = 0;
}

void uart_enable_tx(uart_t uart)
{
    (void)uart;
    GBA_UART->send_enable = 1;
}

void uart_disable_tx(uart_t uart)
{
    (void)uart;
    GBA_UART->send_enable = 0;
}
