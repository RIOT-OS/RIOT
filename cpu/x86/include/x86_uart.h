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
 * Using Communations Port 0 (COM0) to interface with a human being on x86 boards.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__UART__H__
#define CPU__X86__UART__H__

#include "x86_pic.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Early initialization of the UART system, before there are interrupts.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_early_init_uart(void);

/**
 * @brief   Full initialization of the Universal Asynchronous Receiver Transmitter.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_uart(void);

/**
 * @brief       Write out characters to the UART.
 * @param[in]   buf   Buffer to write.
 * @param       len   Length of the buffer.
 * @returns     Actual amount of bytes written, always the same as len.
 *
 * This function blocks with interrupts disabled once the output buffer is full.
 */
ssize_t x86_uart_write(const char *buf, size_t len);

/**
 * @brief        Read in characters to the UART.
 * @param[out]   buf   Buffer to set.
 * @param        len   Length of the buffer.
 * @returns      Actual amount of bytes read, always the same as len.
 *
 * This function blocks with interrupts disabled once the input buffer is full.
 */
ssize_t x86_uart_read(char *buf, size_t len);

#define COM1_PORT (0x03F8)
#define COM2_PORT (0x02F8)
#define COM3_PORT (0x03E8)
#define COM4_PORT (0x02E8)

#define COM1_IRQ (PIC_NUM_RS232_1_3)
#define COM2_IRQ (PIC_NUM_RS232_2_4)
#define COM3_IRQ (PIC_NUM_RS232_1_3)
#define COM4_IRQ (PIC_NUM_RS232_2_4)

/* 115200 baud */
#define BAUD_LL (0x01)
#define BAUD_HL (0x00)

enum uart_port_offs_t {
    /*          DLAB RW */
    THR = 0, /*   0   W  Transmitter Holding Buffer */
    RBR = 0, /*   0  R   Receiver Buffer */
    DLL = 0, /*   1  RW  Divisor Latch Low Byte */
    IER = 1, /*   0  RW  Interrupt Enable Register */
    DLH = 1, /*   1  RW  Divisor Latch High Byte */
    IIR = 2, /*   -  R   Interrupt Identification Register */
    FCR = 2, /*   -  RW  FIFO Control Register */
    LCR = 3, /*   -  RW  Line Control Register */
    MCR = 4, /*   -  RW  Modem Control Register */
    LSR = 5, /*   -  R   Line Status Register */
    MSR = 6, /*   -  R   Modem Status Register */
    SR  = 7, /*   -  RW  Scratch Register */
};

enum ier_t {
    IER_RECV  = 1 << 0, /* Enable Received Data Available Interrupt */
    IER_SEND  = 1 << 1, /* Enable Transmitter Holding Register Empty Interrupt */
    IER_LS    = 1 << 2, /* Enable Receiver Line Status Interrupt */
    IER_MS    = 1 << 3, /* Enable Modem Status Interrupt */
    IER_SLEEP = 1 << 4, /* Enables Sleep Mode (16750) */
    IER_LPM   = 1 << 5, /* Enables Low Power Mode (16750) */
};

enum fcr_t {
    FCR_ENABLE     = 1 << 0, /* Enable FIFOs */
    FCR_CLR_RECV   = 1 << 1, /* Clear Receive FIFO */
    FCR_CLR_SEND   = 1 << 2, /* Clear Transmit FIFO */
    FCR_MDA        = 1 << 3, /* DMA Mode Select */
    FCR_64BYTE     = 1 << 5, /* Enable 64 Byte FIFO (16750) */

    FCR_TRIGGER_1  = 0 << 6, /* 1 byte */
    FCR_TRIGGER_16 = 1 << 6, /* 16 bytes */
    FCR_TRIGGER_32 = 2 << 6, /* 32 bytes */
    FCR_TRIGGER_56 = 3 << 6, /* 56 bytes */
};

enum lcr_t {
    LCR_WORD_BITS_5 = (0) << 0, /* Word length: 5 bits */
    LCR_WORD_BITS_6 = (1) << 0, /* Word length: 6 bits */
    LCR_WORD_BITS_7 = (2) << 0, /* Word length: 7 bits */
    LCR_WORD_BITS_8 = (3) << 0, /* Word length: 8 bits */

    LCR_STOP_BITS_1 = (0) << 2, /* Stop bits: 1 */
    LCR_STOP_BITS_2 = (1) << 2, /* Stop bits: 1.5 or 2 */

    LCR_PAR_NONE    = (0) << 3, /* no parity */
    LCR_PAR_ODD     = (1) << 3, /* odd parity */
    LCR_PAR_EVEN    = (3) << 3, /* even parity */
    LCR_PAR_MARK    = (5) << 3, /* mark (?) */
    LCR_PAR_SPACE   = (7) << 3, /* space (?) */

    LCR_SET_BREAK   = (1) << 6, /* set break enabled */

    LCR_DLAB        = (1) << 7, /* divisor latch access bit */
};

enum mcr_t {
    MCR_DSR      = 1 << 0, /* Data Terminal Ready */
    MCR_RTS      = 1 << 1, /* Request To Send */
    MCR_AUX1     = 1 << 2, /* Auxiliary Output 1 */
    MCR_AUX2     = 1 << 3, /* Auxiliary Output 2 */
    MCR_LOOPBACK = 1 << 4, /* Loopback Mode */
    MCR_AUTOFLOW = 1 << 5, /* Autoflow Control Enabled (16750) */
};

enum iir_t {
    IIR_IPF          = 1 << 0, /* interrupt pending flag */

    IIR_INT_MASK     = 3 << 1,
    IIR_INT_MS       = 0 << 1, /* modem status interrupt (reset: read MSR) */
    IIR_INT_TH       = 1 << 1, /* THR empty interrupt (reset: read IIR or write to THR) */
    IIR_INT_BR       = 2 << 1, /* recv data available interrupt (reset: read RBR) */
    IIR_INT_LS       = 3 << 1, /* line status changed (reset: read LSR) */
    IIR_INT_TO       = 6 << 1, /* timeout interrupt pending (reset: read RBR) */

    IIR_FIFO64       = 1 << 5, /* 64 byte FIFO enabled */

    IIR_FIFO_MASK    = 3 << 6,
    IIR_FIFO_NONE    = 0 << 6, /* no FIFO available */
    IIR_FIFO_RES     = 1 << 6, /* reserved condition */
    IIR_FIFO_MAL     = 2 << 6, /* FIFO enabled, not working */
    IIR_FIFO_ENABLED = 3 << 6, /* FIFO enabled */
};

#ifdef __cplusplus
}
#endif

#endif

/** @} */
