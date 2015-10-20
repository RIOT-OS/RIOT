/*
 * Copyright (C) 2008-2015, Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc2387
 * @{
 *
 * @file
 * @brief       Peripheral UART driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lpc23xx.h"
#include "VIC.h"
#include "kernel.h"
#include "periph/uart.h"

/* for now, we only support one UART device... */
static uart_rx_cb_t _rx_cb;
static void * _cb_arg;

void UART0_IRQHandler(void) __attribute__((interrupt("IRQ")));

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* for now, we only support one UART device and only the RX interrupt */
    if (dev != 0) {
        return -1;
    }

    /* save interrupt context */
    _rx_cb = rx_cb;
    _cb_arg = arg;

    /* power on the UART device */
    PCONP |= PCUART0;
    /* UART0 clock divider is CCLK/8 */
    PCLKSEL0 |= BIT6 + BIT7;
    /* configure to 8N1 */
    U0LCR = 0x83;

    /* Baudrate calculation:
     * BR = PCLK (9 MHz) / (16 x 256 x DLM + DLL) x (1/(DIVADDVAL/MULVAL)) */
    /* TODO: UART Baudrate calculation using the baudrate parameter */
    U0FDR = 0x92;       /* DIVADDVAL = 0010 = 2, MULVAL = 1001 = 9 */
    U0DLM = 0x00;
    U0DLL = 0x04;

    U0LCR = 0x03;       /* DLAB = 0 */
    U0FCR = 0x07;       /* Enable and reset TX and RX FIFO */

    /* install and enable the IRQ handler */
    install_irq(UART0_INT, UART0_IRQHandler, 6);
    U0IER |= BIT0;       /* enable only RX irq */
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(U0LSR & BIT5));
        U0THR = data[i];
    }
}

void UART0_IRQHandler(void)
{
    switch (U0IIR & UIIR_ID_MASK) {
        case UIIR_CTI_INT:              /* Character Timeout Indicator */
        case UIIR_RDA_INT:              /* Receive Data Available */
                do {
                    char c = (char)U0RBR;
                    _rx_cb(_cb_arg, c);
                }
                while (U0LSR & ULSR_RDR);
            break;

        default:
            U0LSR;
            U0RBR;
            break;
    }

    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
