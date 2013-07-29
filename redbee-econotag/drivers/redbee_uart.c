/*
 * redbee_uart.c - UART driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"
#include "uart.h"
#include "gpio.h"
#include "io.h"

#define MOD_ 9999
#define CLK_ 24000000
#define DIV_ 16 /* uart->CON.XTIM = 0 is 16x oversample (datasheet is incorrect) */
#define BAUTRATE_UART1 115200
#define BAUTRATE_UART2 115200

void uart_set_baudrate(volatile struct UART_struct *uart, uint32_t baudrate)
{
    uint64_t inc = 0;

    /* calculate inc following equation 13-1 from datasheet */
    /* multiply by another 10 to get a fixed point*/
    inc = ((uint64_t) baudrate * DIV_ * MOD_ * 10 / CLK_) - 10;
    /* add 5 and div by 10 to get a rounding */
    inc = (inc + 5) / 10;

    /* disable UARTx to set baudrate */
    uart->CONbits.TXE = 0;
    uart->CONbits.RXE = 0;

    /* set baudrate */
    uart->BRbits.INC = inc;
    uart->BRbits.MOD = MOD_;

    /* reenable UARTx again */
    /* uart->CON.XTIM = 0 is 16x oversample (datasheet is incorrect) */
    uart->CONbits.XTIM = 0;
    uart->CONbits.TXE = 1;
    uart->CONbits.RXE = 1;
}

void uart_flow_ctl(volatile struct UART_struct *uart, uint8_t on)
{
    if (on) {
        /* enable flow control */
        if (uart == UART1) {
            /* CTS and RTS directions */
            GPIO->PAD_DIR_SET.U1CTS = 1;
            GPIO->PAD_DIR_RESET.U1RTS = 1;
            /* function select to uart */
            GPIO->FUNC_SEL.U1CTS = 1;
            GPIO->FUNC_SEL.U1RTS = 1;
        }
        else {
            /* UART2 */
            /* CTS and RTS directions */
            GPIO->PAD_DIR_SET.U2CTS = 1;
            GPIO->PAD_DIR_RESET.U2RTS = 1;
            /* function select to uart */
            GPIO->FUNC_SEL.U2CTS = 1;
            GPIO->FUNC_SEL.U2RTS = 1;
        }

        /* enable flow control */
        uart->CONbits.FCE = 1;
    }
    else {
        /* disable flow control */
        uart->CONbits.FCE = 0;

        if (uart == UART1) {
            /* CTS and RTS directions */
            GPIO->PAD_DIR_RESET.U1CTS = 1;
            GPIO->PAD_DIR_RESET.U1RTS = 1;
            /* function select to GPIO */
            GPIO->FUNC_SEL.U1CTS = 3;
            GPIO->FUNC_SEL.U1RTS = 3;
        }
        else {
            /* CTS and RTS directions */
            GPIO->PAD_DIR_RESET.U2CTS = 1;
            GPIO->PAD_DIR_RESET.U2RTS = 1;
            /* function select to GPIO */
            GPIO->FUNC_SEL.U2CTS = 3;
            GPIO->FUNC_SEL.U2RTS = 3;
        }
    }
}

// TODO: clean from u*_(rx|tx)_(head|tail)
void uart_init(volatile struct UART_struct *uart, uint32_t baudrate)
{
    /* enable the uart so we can set the gpio mode */
    /* has to be enabled before setting the function with GPIO->FUNC_SEL */
    uart->CONbits.TXE = 1;
    uart->CONbits.RXE = 1;

    /* interrupt when this or more bytes are free in the tx buffer */
    uart->TXCON = 16;

    if (uart == UART1) {
        /* TX and RX direction */
        GPIO->PAD_DIR_SET.U1TX = 1;
        GPIO->PAD_DIR_RESET.U1RX = 1;

        /* set function selection to UART */
        GPIO->FUNC_SEL.U1TX = 1;
        GPIO->FUNC_SEL.U1RX = 1;

        UART1->CONbits.TXE = 1;                         /*< enable transmit */
        UART1->CONbits.RXE = 1;                         /*< enable receive */
#if UART1_RX_BUFFERSIZE > 32
        UART1->RXCONbits.LVL = 30;                      /*< interrupt when fifo is nearly full */
        //u1_rx_head = 0;
        //u1_rx_tail = 0;
#elif UART1_RX_BUFFERSIZE < 32
        UART1->CONbits.FCE = 1;                         /*< enable flowcontrol */
        UART1->CONbits.MRXR = 1;                        /*< disable Rx interrupt */
        UART1->CTSbits.LVL = UART1_RX_BUFFERSIZE;       /*< drop cts when tx buffer at trigger level */
        GPIO->FUNC_SEL1.U1CTS = 1;                      /*< set GPIO 16 to UART1 CTS */
        GPIO->FUNC_SEL1.U1RTS = 1;                      /*< set GPIO 17 to UART1 RTS */
#else
        UART1->CONbits.MRXR = 1;                        /*< disable rx interrupt */
#endif

        //u1_tx_head = 0;
        //u1_tx_tail = 0;

        //enable_irq(UART1);
        ITC->INTENABLEbits.UART1 = 1;
    }
    else {
        /* UART2 */
        /* TX and RX direction */
        GPIO->PAD_DIR_SET.U2TX = 1;
        GPIO->PAD_DIR_RESET.U2RX = 1;

        /* set function selection to UART */
        GPIO->FUNC_SEL.U2TX = 1;
        GPIO->FUNC_SEL.U2RX = 1;

        UART2->CONbits.TXE = 1;                         /*< enable transmit */
        UART2->CONbits.RXE = 1;                         /*< enable receive */
#if UART2_RX_BUFFERSIZE > 32
        UART2->RXCONbits.LVL = 30;                      /*< interrupt when fifo is nearly full */
        //u2_rx_head = 0;
        //u2_rx_tail = 0;
#elif UART2_RX_BUFFERSIZE < 32
        UART2->CONbits.FCE = 1;                         /*< enable flowcontrol */
        UART2->CONbits.MRXR = 1;                        /*< disable Rx interrupt */
        UART2->CTSbits.LVL = UART2_RX_BUFFERSIZE;       /*< drop cts when tx buffer at trigger level */
        GPIO->FUNC_SEL1.U1CTS = 1;                      /*< set GPIO 16 to UART2 CTS */
        GPIO->FUNC_SEL1.U1RTS = 1;                      /*< set GPIO 17 to UART2 RTS */
#else
        UART2->CONbits.MRXR = 1;                        /*< disable rx interrupt */
#endif

        // u2_tx_head = 0;
        //u2_tx_tail = 0;

        //enable_irq(UART2);
        ITC->INTENABLEbits.UART2 = 1;
    }

    uart_set_baudrate(uart, baudrate);
}

static inline uint32_t uart0_puts(uint8_t *astring, uint32_t length)
{
    uint32_t i = 0;

    for (; i < length; i++) {
        uart1_putc(astring[i]);
    }

    return i;
}

void stdio_flush(void)
{
    ITC->INTENABLEbits.UART1 = 0;
    ITC->INTENABLEbits.UART2 = 0;

    ITC->INTENABLEbits.UART1 = 1;
    ITC->INTENABLEbits.UART2 = 1;
    /** taken from msba2-uart0.c
    U0IER &= ~BIT1;                             // disable THRE interrupt
    while(running) {
        while(!(U0LSR & (BIT5|BIT6))){};        // transmit fifo
        fifo=0;
        push_queue();                           // dequeue to fifo
    }
    U0IER |= BIT1;                              // enable THRE interrupt
    */
}


int fw_puts(char *astring, int length)
{
    return uart0_puts((uint8_t *) astring, (uint32_t) length);
}

int bl_uart_init(void)
{
    uart_init(UART1, BAUTRATE_UART1);
    uart_init(UART2, BAUTRATE_UART2);
}