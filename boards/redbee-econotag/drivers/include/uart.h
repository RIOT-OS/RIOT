/*
 * uart.h - UART driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------*/
/* UART */
#define UART1_BASE      (0x80005000)
#define UART2_BASE      (0x8000B000)

struct UART_struct {
    union {
        uint32_t CON;
        struct UART_CON {
            uint32_t TXE: 1;        /*< Tx Enable */
            uint32_t RXE: 1;        /*< Rx Enable */
            uint32_t PEN: 1;        /*< Parity Enable */
            uint32_t EP: 1;         /*< Even Parity (1=Odd, 0=Even) */
            uint32_t ST2: 1;        /*< Enable 2 Stop Bits */
            uint32_t SB: 1;         /*< Send Break */
            uint32_t CONTX: 1;      /*< Continuous Tx (Test Mode) */
            uint32_t TXOENB: 1;     /*< TXD Outbut Disable */
            uint32_t : 2;           /*< reserved */
            uint32_t XTIM: 1;       /*< Times of Oversampling */
            uint32_t FCP: 1;        /*< Flow Control Polarity */
            uint32_t FCE: 1;        /*< Flow Control Enable */
            uint32_t MTXR: 1;       /*< enable/disable TxRDY Interrupts */
            uint32_t MRXR: 1;       /*< enable/disable RxRDY Interrupts */
            uint32_t TST: 1;        /*< Test Loop-Back */
            uint32_t : 16;          /*< reserved */
        } CONbits;
    };
    union {
        uint32_t STAT;
        struct UART_STAT {
            uint32_t SE: 1;         /*< Start Bit Error */
            uint32_t PE: 1;         /*< Parity Bit Error */
            uint32_t FE: 1;         /*< Frame/Stop Bit Error */
            uint32_t TOE: 1;        /*< Tx FIFO Overrun Error */
            uint32_t ROE: 1;        /*< Rx FIFO Overrun Error */
            uint32_t RUE: 1;        /*< Rx FIFO Underrun Error */
            uint32_t RXRDY: 1;      /*< Receiver is causing Interrupts */
            uint32_t TXRDY: 1;      /*< Transmitter is causing Interrupts */
            uint32_t : 24;
        } USTATbits;
    };
    union {
        uint32_t DATA;
        struct UART_DATA {
            uint32_t DATA: 8;
            uint32_t : 24;
        } DATAbits;
    };
    union {
        uint32_t RXCON;
        struct UART_URXCON {
            uint32_t LVL: 6;
            uint32_t : 26;
        } RXCONbits;
    };
    union {
        uint32_t TXCON;
        struct UART_TXCON {
            uint32_t LVL: 6;
            uint32_t : 26;
        } TXCONbits;
    };
    union {
        uint32_t CTS;
        struct UART_CTS {
            uint32_t LVL: 5;
            uint32_t : 27;
        } CTSbits;
    };

    uint32_t BR;                    /*< BR is 32bit access only */
};

static volatile struct UART_struct *const UART1 = (void *)(UART1_BASE);
static volatile struct UART_struct *const UART2 = (void *)(UART2_BASE);

void uart_init(volatile struct UART_struct *uart, uint32_t baud);
void uart_set_baudrate(volatile struct UART_struct *uart, uint32_t baud);
void uart_flow_ctl(volatile struct UART_struct *uart, uint8_t on);

#undef UART0_BUFSIZE
#define UART0_BUFSIZE (32)

void uart1_putc(uint8_t c);
uint8_t uart1_getc(void);

void uart2_putc(uint8_t c);
uint8_t uart2_getc(void);

#ifdef __cplusplus
}
#endif

#endif
