/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"

/**
 * @brief   Bit mask for the fractional part of the baudrate
 */
#define FRAC_BITS           (6U)
#define FRAC_MASK           (0x3f)

/**
 * @brief   Get the enable mask depending on enabled HW flow control
 */
#if UART_HW_FLOW_CONTROL
#define ENABLE_MASK         (UART_CTSEN | UART_CTL_RTSEN | \
                             UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN)
#else
#define ENABLE_MASK         (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN)
#endif

/**
 * @brief allocate memory to store callback functions
 */
static uart_isr_ctx_t ctx[UART_NUMOF];


int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* make sure the uart device is valid */
    if (uart != 0) {
        return -1;
    }

    /* enable clocks: serial power domain and UART */
    PRCM->PDCTL0SERIAL = 1;
    while (!(PRCM->PDSTAT0 & PDSTAT0_SERIAL_ON)) ;
    uart_poweron(uart);

    /* disable and reset the UART */
    UART->CTL = 0;

    /* save context */
    ctx[0].rx_cb = rx_cb;
    ctx[0].arg = arg;

    /* configure pins */
    IOC->CFG[UART_TX_PIN] =  IOCFG_PORTID_UART0_TX;
    IOC->CFG[UART_RX_PIN] = (IOCFG_PORTID_UART0_RX | IOCFG_INPUT_ENABLE);
#if UART_HW_FLOW_CONTROL
    IOC->CFG[UART_RTS_PIN] =  IOCFG_PORTID_UART0_RTS;
    IOC->CFG[UART_CTS_PIN] = (IOCFG_PORTID_UART0_CTS | IOCFG_INPUT_ENABLE);
#endif

    /* calculate baud-rate */
    uint32_t tmp = (CLOCK_CORECLOCK * 4);
    tmp += (baudrate / 2);
    tmp /= baudrate;
    UART->IBRD = (tmp >> FRAC_BITS);
    UART->FBRD = (tmp & FRAC_MASK);

    /* configure line to 8N1 mode, LRCH must be written after IBRD and FBRD! */
    UART->LCRH = UART_LCRH_WLEN_8;

    /* enable the RX interrupt */
    UART->IMSC = UART_IMSC_RXIM;
    NVIC_EnableIRQ(UART0_IRQN);

    /* start the UART */
    UART->CTL = ENABLE_MASK;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (UART->FR & UART_FR_TXFF) {}
        UART->DR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    PRCM->UARTCLKGR = 1;
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    UART->CTL = ENABLE_MASK;
}

void uart_poweroff(uart_t uart)
{
    UART->CTL = 0;

    PRCM->UARTCLKGR = 0;
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

}

void isr_uart(void)
{
    /* remember pending interrupts */
    uint32_t mis = UART->MIS;
    /* clear them */
    UART->ICR = mis;

    /* read received byte and pass it to the RX callback */
    if (mis & UART_MIS_RXMIS) {
        ctx[0].rx_cb(ctx[0].arg, (uint8_t)UART->DR);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
