/*
 * uart.h - UART driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#ifndef UART_H
#define UART_H

/*-----------------------------------------------------------------*/
/* UART */
#define UART1_BASE      (0x80005000)
#define UART2_BASE      (0x8000B000)

struct UART_struct {
    union {
        uint32_t CON;
        struct UART_CON {
            uint32_t : 16;
            uint32_t TST: 1;
            uint32_t MRXR: 1;
            uint32_t MTXR: 1;
            uint32_t FCE: 1;
            uint32_t FCP: 1;
            uint32_t XTIM: 1;
            uint32_t : 2;
            uint32_t TXOENB: 1;
            uint32_t CONTX: 1;
            uint32_t SB: 1;
            uint32_t ST2: 1;
            uint32_t EP: 1;
            uint32_t PEN: 1;
            uint32_t RXE: 1;
            uint32_t TXE: 1;
        } CONbits;
    };
    union {
        uint32_t STAT;
        struct UART_STAT {
            uint32_t : 24;
            uint32_t TXRDY: 1;
            uint32_t RXRDY: 1;
            uint32_t RUE: 1;
            uint32_t ROE: 1;
            uint32_t TOE: 1;
            uint32_t FE: 1;
            uint32_t PE: 1;
            uint32_t SE: 1;
        } USTATbits;
    };
    union {
        uint32_t DATA;
        struct UART_DATA {
            uint32_t : 24;
            uint32_t DATA: 8;
        } DATAbits;
    };
    union {
        uint32_t RXCON;
        struct UART_URXCON {
            uint32_t : 26;
            uint32_t LVL: 6;
        } RXCONbits;
    };
    union {
        uint32_t TXCON;
        struct UART_TXCON {
            uint32_t : 26;
            uint32_t LVL: 6;
        } TXCONbits;
    };
    union {
        uint32_t CTS;
        struct UART_CTS {
            uint32_t : 27;
            uint32_t LVL: 5;
        } CTSbits;
    };
    union {
        uint32_t BR;
        struct UART_BR {
            uint32_t INC: 16;
            uint32_t MOD: 16;
        } BRbits;
    };
};

static volatile struct UART_struct *const UART1 = (void *)(UART1_BASE);
static volatile struct UART_struct *const UART2 = (void *)(UART2_BASE);

void uart_init(volatile struct UART_struct *uart, uint32_t baud);
void uart_set_baudrate(volatile struct UART_struct *uart, uint32_t baud);
void uart_flow_ctl(volatile struct UART_struct *uart, uint8_t on);


#define UART0_BUFSIZE (32)

/* The mc1322x has a 32 byte hardware FIFO for transmitted characters.
 * Currently it is always filled from a larger RAM buffer. It would be
 * possible to eliminate that overhead by filling directly from a chain
 * of data buffer pointers, but printf's would be not so easy.
 */
#define UART1_TX_BUFFERSIZE 1024
extern volatile uint32_t  u1_tx_head, u1_tx_tail;
void uart1_putc(uint8_t c);

/* The mc1322x has a 32 byte hardware FIFO for received characters.
 * If a larger rx buffersize is specified the FIFO will be extended into RAM.
 * RAM transfers will occur on interrupt when the FIFO is nearly full.
 * If a smaller buffersize is specified hardware flow control will be
 * initiated at that FIFO level.
 * Set to 32 for no flow control or RAM buffer.
 */
#define UART1_RX_BUFFERSIZE 128
#if UART1_RX_BUFFERSIZE > 32
extern volatile uint32_t  u1_rx_head, u1_rx_tail;
#define uart1_can_get() ((u1_rx_head!=u1_rx_tail) || (*UART1_URXCON > 0))
#else
#define uart1_can_get() (*UART1_URXCON > 0)
#endif
uint8_t uart1_getc(void);


#define UART2_TX_BUFFERSIZE 1024
extern volatile uint32_t  u2_tx_head, u2_tx_tail;
void uart2_putc(uint8_t c);

#define UART2_RX_BUFFERSIZE 128
#if UART2_RX_BUFFERSIZE > 32
extern volatile uint32_t  u2_rx_head, u2_rx_tail;
#define uart2_can_get() ((u2_rx_head!=u2_rx_tail) || (*UART2_URXCON > 0))
#else
#define uart2_can_get() (*UART2_URXCON > 0)
#endif
uint8_t uart2_getc(void);

#endif