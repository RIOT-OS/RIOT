/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       UART reading and writing.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "x86_ports.h"
#include "x86_uart.h"

#include <cpu.h>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void x86_early_init_uart(void)
{
    outb(UART_PORT + IER, 0x00); /* Disable all interrupts */

    outb(UART_PORT + LCR, LCR_DLAB);
    outb(UART_PORT + DLL, BAUD_LL); /* Set divisor (lo byte) */
    outb(UART_PORT + DLH, BAUD_HL); /*             (hi byte) */
    outb(UART_PORT + LCR, LCR_WORD_BITS_8 | LCR_PAR_NONE | LCR_STOP_BITS_1);
    outb(UART_PORT + FCR, FCR_ENABLE | FCR_CLR_RECV | FCR_CLR_SEND | FCR_TRIGGER_16);
    outb(UART_PORT + MCR, MCR_DSR | MCR_RTS | MCR_AUX2);
}

static bool is_output_empty(void)
{
    uint8_t data = inb(UART_PORT + LSR);
    return (data & 0x20) != 0;
}

static bool is_input_empty(void)
{
    uint8_t data = inb(UART_PORT + LSR);
    return (data & 0x01) == 0;
}

ssize_t x86_uart_write(const char *buf, size_t len)
{
    if (!UART_PORT) {
        return -1;
    }
    (void) buf;

    size_t written = 0;
    while (written < len) {
        while (!is_output_empty()) {
            __asm__ volatile ("pause");
        }
        outb(UART_PORT + THR, buf[written]);
        ++written;
    }
    return written;
}

ssize_t x86_uart_read(char *buf, size_t len)
{
    if (!UART_PORT) {
        return -1;
    }

    size_t read = 0;
    while (read < len) {
        while (!is_input_empty()) {
            __asm__ volatile ("pause");
        }
        buf[read] = inb(UART_PORT + RBR);
        ++read;
    }
    return read;
}

void x86_init_uart(void)
{
}
