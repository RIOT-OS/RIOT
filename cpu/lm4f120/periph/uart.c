/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
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
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

/**
 * The list of UART peripherals.
 */
static const unsigned long g_ulUARTPeriph[3] =
{
    SYSCTL_PERIPH_UART0,
    SYSCTL_PERIPH_UART1,
    SYSCTL_PERIPH_UART2
};

/**
 * The list of all possible base address of the console UART
 */
static const unsigned long g_ulUARTBase[3] =
{
    UART0_BASE,
    UART1_BASE,
    UART2_BASE
};

/**
 * The list of possible interrupts for the console UART.
 */
static const unsigned long g_ulUARTInt[3] =
{
    INT_UART0,
    INT_UART1,
    INT_UART2
};

static int init_base(uart_t uart, uint32_t baudrate);

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* Check the arguments */
    assert(uart == 0);
    /* Check to make sure the UART peripheral is present */
    if(!ROM_SysCtlPeripheralPresent(SYSCTL_PERIPH_UART0)){
        return -1;
    }

    int res = init_base(uart, baudrate);
    if(res < 0){
        return res;
    }

/* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

/*  ulBase = g_ulUARTBase[uart]; */
    switch (uart){
#if UART_0_EN
        case UART_0:
            ROM_UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);
            ROM_UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
            ROM_UARTFIFOEnable(UART0_BASE);

            /* Enable the UART interrupt */
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            /* Enable RX interrupt */
            UART0_IM_R = UART_IM_RXIM | UART_IM_RTIM;
            break;
#endif
#if UART_1_EN
        case UART_1:
            /* Enable the UART interrupt */
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            break;
#endif
    }
    return 0;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    switch(uart){
#if UART_0_EN
        case UART_0:
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
#endif
        }
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        ROM_UARTCharPut(UART0_BASE, (char)data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    ROM_UARTEnable(UART0_BASE);
}

void uart_poweroff(uart_t uart)
{
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

    /* Are we interrupted due to a recieved character */
    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(ROM_UARTCharsAvail(UART0_BASE))
        {
            char cChar;
            long lChar;
            lChar = ROM_UARTCharGetNonBlocking(UART0_BASE);
            cChar = (unsigned char)(lChar & 0xFF);
            config[UART_0].rx_cb(config[UART_0].arg, cChar);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
