/*
 * SPDX-FileCopyrightText: 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lm4f120
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file        uart.c
 * @brief       Implementation of the low-level UART driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* The only implemented UART device number for this cpu. */
#define _UART_DEV_NUM   0

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

static int init_base(uart_t uart, uint32_t baudrate);

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* Check the arguments */
    /* Only one UART is supported, assert that is what is used. */
    assert(uart == _UART_DEV_NUM);
    (void) uart;
    /* Check to make sure the UART peripheral is present */
    if (!ROM_SysCtlPeripheralPresent(SYSCTL_PERIPH_UART0)) {
        return UART_NODEV;
    }

    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return res;
    }

/* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    ROM_UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);
    ROM_UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
    ROM_UARTFIFOEnable(UART0_BASE);

    /* Enable the UART interrupt */
    NVIC_EnableIRQ(UART_0_IRQ_CHAN);
    /* Enable RX interrupt */
    UART0_IM_R = UART_IM_RXIM | UART_IM_RTIM;
    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
        case _UART_DEV_NUM:
            ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
            ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
            ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
            ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

            ROM_UARTDisable(UART0_BASE);
            ROM_UARTConfigSetExpClk(UART0_BASE,ROM_SysCtlClockGet(), baudrate,
                    (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                     UART_CONFIG_WLEN_8));
            ROM_UARTEnable(UART0_BASE);
            break;
        default:
            return UART_NODEV;
        }
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    /* Only one UART is supported, assert that is what is used. */
    assert(uart == _UART_DEV_NUM);
    (void) uart;
    for (size_t i = 0; i < len; i++) {
        ROM_UARTCharPut(UART0_BASE, (char)data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    /* Only one UART is supported, assert that is what is used. */
    assert(uart == _UART_DEV_NUM);
    (void) uart;
    ROM_UARTEnable(UART0_BASE);
}

void uart_poweroff(uart_t uart)
{
    /* Only one UART is supported, assert that is what is used. */
    assert(uart == _UART_DEV_NUM);
    (void) uart;
    ROM_UARTDisable(UART0_BASE);
}

/**
 * The UART interrupt handler.
 */
void isr_uart0(void)
{
    unsigned long ulStatus;

    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    /* Are we interrupted due to a received character */
    if (ulStatus & (UART_INT_RX | UART_INT_RT)) {
        while (ROM_UARTCharsAvail(UART0_BASE)) {
            long lchar = ROM_UARTCharGetNonBlocking(UART0_BASE);
            config[_UART_DEV_NUM].rx_cb(config[_UART_DEV_NUM].arg,
                    (uint8_t)lchar);
        }
    }
    cortexm_isr_end();
}
