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
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard the file in case no UART is defined */
#if UART_0_EN

/**
 * @brief Struct holding the configuration data for a UART device
 * @{
 */
typedef struct {
    uart_rx_cb_t rx_cb;         /**< receive callback */
    uart_tx_cb_t tx_cb;         /**< transmit callback */
    void *arg;                  /**< callback argument */
} uart_conf_t;
/**@}*/

/**
 * @brief UART device configurations
 */
static uart_conf_t config[UART_NUMOF];

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

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    // Check the arguments
    ASSERT(uart == 0);
    // Check to make sure the UART peripheral is present
    if(!ROM_SysCtlPeripheralPresent(SYSCTL_PERIPH_UART0)){
        return -1;
    }

    int res = uart_init_blocking(uart, baudrate);
    if(res < 0){
        return res;
    }

/* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

//  ulBase = g_ulUARTBase[uart];
    switch (uart){
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);

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
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);
            /* Enable the UART interrupt */
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            break;
#endif
    }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
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

void uart_tx_begin(uart_t uart)
{
    uart_write(uart, '\0');
    UART0_IM_R |= UART_IM_TXIM;
}

int uart_write(uart_t uart, char data)
{
    int ret=ROM_UARTCharPutNonBlocking(UART0_BASE, data);
    return ret;
}

int uart_read_blocking(uart_t uart, char *data)
{
    *data = (char)ROM_UARTCharGet(UART0_BASE);
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    ROM_UARTCharPut(UART0_BASE, data);
    return 1;
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
void isr_usart0(void)
{
    unsigned long ulStatus;
    char cChar;
    long lChar;

    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    // Are we interrupted due to TX done
    if(ulStatus & UART_INT_TX)
    {
        if (config[UART_0].tx_cb(config[UART_0].arg) == 0){
            UART0_IM_R &= ~UART_IM_TXIM;
        }
    }

    // Are we interrupted due to a recieved character
    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(ROM_UARTCharsAvail(UART0_BASE))
        {
            lChar = ROM_UARTCharGetNonBlocking(UART0_BASE);
            cChar = (unsigned char)(lChar & 0xFF);
            config[UART_0].rx_cb(config[UART_0].arg, cChar);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /*UART_0_EN*/
/** @} */
