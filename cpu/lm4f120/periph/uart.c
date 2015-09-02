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
#if UART_NUMOF

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
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param ulBase        the UART device that triggered the ISR
 * @param ulRxInt       the RX Interrupt Mask of the UART Device that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, unsigned long ulBase, unsigned long ulRxInt);

/**
 * @brief UART device configurations
 */
static uart_conf_t config[UART_NUMOF];


/**
 * The list of UART peripherals.
 */
static unsigned long g_ulUARTPeriph[2] =
{
    SYSCTL_PERIPH_UART0,
    SYSCTL_PERIPH_UART1
};

/**
 * The list of all possible base address of the console UART
 */
static const unsigned long g_ulUARTBase[2] =
{
    UART0_BASE,
    UART1_BASE
};

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /*The base address of the chosen UART */
    unsigned long ulBase = 0;

    /* Check the arguments */
    ASSERT((uart == 0) || (uart == 1));
    /* Check to make sure the UART peripheral is present */
    if(!ROM_SysCtlPeripheralPresent(g_ulUARTPeriph[uart])){
        return -1;
    }

    /* Select the base address of the UART */
    ulBase = g_ulUARTBase[uart];

    int res = uart_init_blocking(uart, baudrate);
    if(res < 0){
        return res;
    }

/* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;


    switch (uart){
#if UART_0_EN
        case UART_0:
            NVIC_SetPriority(UART_0_IRQ_CHAN, UART_IRQ_PRIO);

            ROM_UARTTxIntModeSet(ulBase, UART_TXINT_MODE_EOT);
            ROM_UARTFIFOLevelSet(ulBase, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
            ROM_UARTFIFOEnable(ulBase);

            /* Enable the UART interrupt */
            NVIC_EnableIRQ(UART_0_IRQ_CHAN);
            /* Enable RX interrupt */
            UART_0_IM = UART_IM_RXIM | UART_IM_RTIM;
            break;
#endif
#if UART_1_EN
        case UART_1:
            NVIC_SetPriority(UART_1_IRQ_CHAN, UART_IRQ_PRIO);

            ROM_UARTTxIntModeSet(ulBase, UART_TXINT_MODE_EOT);
            ROM_UARTFIFOLevelSet(ulBase, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
            ROM_UARTFIFOEnable(ulBase);

            /* Enable the UART interrupt */
            NVIC_EnableIRQ(UART_1_IRQ_CHAN);
            /* Enable RX interrupt */
            UART_1_IM = UART_IM_RXIM | UART_IM_RTIM;
            break;
#endif
    }
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    /*The base address of the chosen UART */
    unsigned long ulBase = g_ulUARTBase[uart];

    ROM_SysCtlPeripheralEnable(g_ulUARTPeriph[uart]);
    switch(uart){
#if UART_0_EN
        case UART_0:
            ROM_SysCtlPeripheralEnable(UART_0_PORT);
            ROM_GPIOPinConfigure(UART_0_RX_PIN);
            ROM_GPIOPinConfigure(UART_0_TX_PIN);
            ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            break;
#endif
#if UART_1_EN
        case UART_1:
            ROM_SysCtlPeripheralEnable(UART_1_PORT);
            ROM_GPIOPinConfigure(UART_1_RX_PIN);
            ROM_GPIOPinConfigure(UART_1_TX_PIN);
            ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            break;
#endif
    }
    ROM_UARTDisable(ulBase);
    ROM_UARTConfigSetExpClk(ulBase,UART_CLK, baudrate,
            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
    ROM_UARTEnable(ulBase);

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    uart_write(uart, '\0');
    switch(uart){
#if UART_0_EN
        case UART_0:
            UART_0_IM |= UART_IM_TXIM;
            break;
#endif
#if UART_1_EN
        case UART_1:
            UART_1_IM |= UART_IM_TXIM;
            break;
#endif
    }
}

int uart_write(uart_t uart, char data)
{
    int ret=0;
    switch(uart){
#if UART_0_EN
        case UART_0:
            ret=ROM_UARTCharPutNonBlocking(UART_0_DEV, data);
            break;
#endif
#if UART_1_EN
        case UART_1:
            ret=ROM_UARTCharPutNonBlocking(UART_1_DEV, data);
            break;
#endif
    }
    return ret;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch(uart){
#if UART_0_EN
        case UART_0:
            *data = (char)ROM_UARTCharGet(UART_0_DEV);
            break;
#endif
#if UART_1_EN
        case UART_1:
            *data = (char)ROM_UARTCharGet(UART_1_DEV);
            break;
#endif
    }
    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch(uart){
#if UART_0_EN
        case UART_0:
            ROM_UARTCharPut(UART_0_DEV, data);
            break;
#endif
#if UART_1_EN
        case UART_1:
            ROM_UARTCharPut(UART_1_DEV, data);
            break;
#endif
    }
    return 1;
}

void uart_poweron(uart_t uart)
{
    switch(uart){
#if UART_0_EN
        case UART_0:
            ROM_UARTEnable(UART_0_DEV);
            break;
#endif
#if UART_1_EN
        case UART_1:
            ROM_UARTEnable(UART_1_DEV);
            break;
#endif
    }
}

void uart_poweroff(uart_t uart)
{
    switch(uart){
#if UART_0_EN
        case UART_0:
            ROM_UARTDisable(UART_0_DEV);
            break;
#endif
#if UART_1_EN
        case UART_1:
            ROM_UARTDisable(UART_0_DEV);
            break;
#endif
    }
}

/**
 * The UART interrupt handler.
 */
#if UART_0_EN
void UART_0_ISR(void)
{
    irq_handler(UART_0, UART_0_DEV, UART_0_IM);
}
#endif

#if UART_1_EN
void UART_1_ISR(void)
{
    irq_handler(UART_1, UART_1_DEV, UART_1_IM);
}
#endif


static inline void irq_handler(uart_t uartnum, unsigned long ulBase, unsigned long ulRxInt)
{
    unsigned long ulStatus;

    ulStatus = ROM_UARTIntStatus(ulBase, true);
    ROM_UARTIntClear(ulBase, ulStatus);

    /* Are we interrupted due to TX done */
    if(ulStatus & UART_INT_TX)
    {
        if (config[uartnum].tx_cb(config[uartnum].arg) == 0){
            ulRxInt &= ~UART_IM_TXIM;
        }
    }

    /* Are we interrupted due to a recieved character */
    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(ROM_UARTCharsAvail(ulBase))
        {
            char cChar;
            long lChar;
            lChar = ROM_UARTCharGetNonBlocking(ulBase);
            cChar = (unsigned char)(lChar & 0xFF);
            config[uartnum].rx_cb(config[uartnum].arg, cChar);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /*UART_NUMOF*/
/** @} */
