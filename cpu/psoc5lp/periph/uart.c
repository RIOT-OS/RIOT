/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_psoc5lp
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the PSoC 5LP
 *
 * @author      Murat Cakmak <mail@muratcakmak.net>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

#include <project.h>

/* guard the file in case no UART is defined */
#if UART_0_EN

/*************************** MACRO DEFINITIONS ********************************/

/**
 * @brief Calculates Divider value of UART Clock to set Baud Rate
 */
#define UART_CLK_DIVIDER_VALUE(baudrate) (BCLK__BUS_CLK__HZ / ((baudrate) * 8))

/*************************** TYPE DEFINITIONS ********************************/
/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    uart_rx_cb_t rx_cb;         /**< receive callback */
    uart_tx_cb_t tx_cb;         /**< transmit callback */
    void *arg;                  /**< callback argument */
} uart_conf_t;

/******************************* VARIABLES ***********************************/
/**
 * @brief UART device configurations
 */
static uart_conf_t config[UART_NUMOF];

/**************************** PRIVATE FUNCTIONS ******************************/
/**
 * @brief Handler function for UART TX Interrupt
 *
 */
CY_ISR(UART0_TX_IRQHandler)
{
    /* Check 'TX FIFO Empty' Interrupt to ensure all data is sent */
    if (UART0_ReadTxStatus() & UART0_TX_STS_FIFO_EMPTY)
    {
        /* Disable TX Interrupt If client request */
        if (config[UART_0].tx_cb(config[UART_0].arg) == 0) {
            UART0_TX_IRQn_Disable();
        }
    }
    
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**
 * @brief Handler function for UART RX Interrupt
 */
CY_ISR(UART0_RX_IRQHandler)
{
    /* Check 'FIFO Not Empty' Interrupt to ensure any data is received. */
    if (UART0_ReadRxStatus() & UART0_RX_STS_FIFO_NOTEMPTY)
    {
        /* Read received data and notify client */
        char data = UART0_ReadRxData();
        config[UART_0].rx_cb(config[UART_0].arg, data);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**************************** PUBLIC FUNCTIONS *******************************/

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{    
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }
 
    /* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    switch (uart) {

        case UART_0:
        
            /* configure and enable global device interrupts */
            if (tx_cb != NULL)
            {
                /* Enable TX Interrupt */
                /* TODO : DEFECT. System is halting when TX Interrupt is enabled.
                 * TX Interrupt works fine individually(tested in main.c) but in 
                 * this module, this problem should be fixed.
                 * RIOT not using TX Interrupt so Fix is postponed to next 
                 * revisions.
                 */
                UART0_TX_IRQn_StartEx(UART0_TX_IRQHandler);
            }
            
            
            if (rx_cb != NULL)
            {
                /* Enable RX Interrupt */
                UART0_RX_IRQn_StartEx(UART0_RX_IRQHandler);
            }
           
            UART0_Start();
            break;
        default:
            return -1;
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
        case UART_0:
            /* Set Divider value of UART CLK for Baudrate */
            CLK_UART_SetDividerValue(UART_CLK_DIVIDER_VALUE(baudrate));
            break;
        default:
            return -1;
    }

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
        case UART_0:
            /* enable TX interrupt */
            UART0_TX_IRQn_Enable();
            break;
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            UART0_WriteTxData(data);
            break;
        default:
            return -1;
    }

    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
        case UART_0:
            /* Wait until a byte received */
            while (!(UART0_RXSTATUS_REG & UART0_RX_STS_FIFO_NOTEMPTY));
            
            *data = UART0_ReadRxData();            
            break;
        default:
            return -1;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            /* Wait until all previous bytes are sent */
            while(!(UART0_TXSTATUS_REG & UART0_TX_STS_FIFO_EMPTY));

            UART0_TXDATA_REG = data;
            break;
        default:
            return -1;
    }

    return 1;
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
        case UART_0:
            CLK_UART_Start();
            UART0_Start();
            break;
    }
}

void uart_poweroff(uart_t uart)
{
    switch (uart) {
        case UART_0:
            UART0_Stop();
            CLK_UART_Stop();
            break;
    }
}

#endif /* UART_0_EN */
