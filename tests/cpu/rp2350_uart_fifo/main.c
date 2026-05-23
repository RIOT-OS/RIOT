/*
 * SPDX-FileCopyrightText: 2026 Mert Cakir <mert-cakir@outlook.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   RP2350 UART FIFO / ISR drain self-test (internal loopback).
 *
 * Sends a 64-byte burst on UART_DEV(1) at 115200 baud and verifies all
 * 64 bytes come back via the RX callback. Uses the UART internal
 * loopback path so the test does not depend on external GP8/GP9
 * wiring or the board console bridge.
 *
 * Without the RX FIFO enabled (FEN), the 1-byte holding register
 * cannot absorb a 64-byte burst at 115200 baud; bytes get dropped.
 * With the FIFO + full ISR drain in cpu/rp2350_common/periph/uart,
 * all 64 bytes are buffered and delivered to the rx_cb.
 *
 * @author  Mert Cakir <mert-cakir@outlook.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "periph/uart.h"
#include "periph_conf.h"
#include "regs/uart.h"

#define TEST_UART       UART_DEV(1)
#define TEST_UART_IDX   1
#define TEST_BAUD       115200
#define BURST_SIZE      64

static volatile size_t rx_count;
static uint8_t rx_buf[BURST_SIZE];

static void rx_cb(void *arg, uint8_t byte)
{
    (void)arg;
    if (rx_count < sizeof(rx_buf)) {
        rx_buf[rx_count++] = byte;
    }
}

int main(void)
{
    puts("rp2350 uart fifo loopback test");

    if (uart_init(TEST_UART, TEST_BAUD, rx_cb, NULL) != UART_OK) {
        puts("FAIL: uart_init");
        return 1;
    }

    /* Drain any residual RX FIFO data (defensive; uart_init's drain
     * should already have done this with the updated driver). */
    UART0_Type *dev = uart_config[TEST_UART_IDX].dev;
    while (!(dev->UARTFR & UART_UARTFR_RXFE_BITS)) {
        (void)dev->UARTDR;
    }
    dev->UARTCR |= UART_UARTCR_LBE_BITS;

    uint8_t tx_buf[BURST_SIZE];
    for (size_t i = 0; i < BURST_SIZE; i++) {
        tx_buf[i] = (uint8_t)(i ^ 0x5a);
    }

    rx_count = 0;
    uart_write(TEST_UART, tx_buf, BURST_SIZE);

    /* Give the ISR time to drain the last bytes from the RX FIFO. The
     * 64-byte transfer itself takes about 5.5 ms at 115200 baud. */
    for (volatile uint32_t i = 0; i < 200000; i++) { }

    if (rx_count != BURST_SIZE) {
        printf("FAIL: expected %u bytes, got %u\n",
               (unsigned)BURST_SIZE, (unsigned)rx_count);
        return 1;
    }

    if (memcmp(tx_buf, rx_buf, BURST_SIZE) != 0) {
        puts("FAIL: byte mismatch");
        return 1;
    }

    printf("PASS: %u bytes round-tripped via external loopback\n",
           (unsigned)BURST_SIZE);
    puts("ALL TESTS PASSED");
    return 0;
}
