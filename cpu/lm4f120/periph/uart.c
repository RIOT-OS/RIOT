/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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


/**
 * @brief UART module size
 */
 #define UART_MOD_SIZE  (UART1_BASE - UART0_BASE)

 /**
  * @brief UART base depending on dev
  */
 #define GET_UART_BASE(x) (UART0_BASE + (x * UART_MOD_SIZE))

 /**
  * @brief UART_IM_R register address depending on dev
  */
 #define GET_UART_IM_R(x) (&UART0_IM_R + (x * UART_MOD_SIZE))

/**
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);
    /* Check the arguments */

    /* Derive UART parameters based off dev */
    unsigned long uart_base = GET_UART_BASE(uart_config[uart].dev);
    volatile unsigned long * uart_im_r = GET_UART_IM_R(uart_config[uart].dev);
    unsigned long uart_sysctl = SYSCTL_PERIPH_UART0 | (1<<uart_config[uart].dev);

    /* Check to make sure the UART peripheral is present */
    if(!ROM_SysCtlPeripheralPresent(uart_sysctl)){
        return UART_NODEV;
    }

    ROM_SysCtlPeripheralEnable(uart_sysctl);
    ROM_SysCtlPeripheralEnable(uart_config[uart].gpio_sysctl);

    /* Configure the Rx and Tx pins. If no callback function is defined,
     * the UART should be initialised in Tx only mode.
     */
    if (rx_cb) {
        ROM_GPIOPinConfigure(uart_config[uart].pins.rx);
        ROM_GPIOPinTypeUART(uart_config[uart].gpio_port,
          uart_config[uart].pins.mask_tx | uart_config[uart].pins.mask_rx );
        /* save callbacks */
        config[uart].rx_cb = rx_cb;
        config[uart].arg = arg;
    }
    else {
        ROM_GPIOPinTypeUART(uart_config[uart].gpio_port,
          uart_config[uart].pins.mask_tx);
    }
    ROM_GPIOPinConfigure(uart_config[uart].pins.tx);

    ROM_UARTDisable(uart_base);
    ROM_UARTConfigSetExpClk(uart_base, ROM_SysCtlClockGet(), baudrate,
            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
             UART_CONFIG_WLEN_8));

    ROM_UARTEnable(uart_base);

    ROM_UARTTxIntModeSet(uart_base, uart_config[uart].uart_txint_mode);
    ROM_UARTFIFOLevelSet(uart_base,uart_config[uart].uart_fifo_tx,
        uart_config[uart].uart_fifo_rx);
    ROM_UARTFIFOEnable(uart_base);

    /* Enable the UART interrupt */
    NVIC_EnableIRQ(uart_config[uart].uart_irq_chan);

    /* Enable RX interrupt */
    (*uart_im_r) = (UART_IM_RXIM | UART_IM_RTIM);

    return UART_OK;
}


void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        ROM_UARTCharPut(GET_UART_BASE(uart_config[uart].dev), (char)data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    ROM_UARTEnable(GET_UART_BASE(uart_config[uart].dev));
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    ROM_UARTDisable(GET_UART_BASE(uart_config[uart].dev));
}

/**
 * The UART interrupt handler.
 */
void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);

    unsigned long ulStatus;

    ulStatus = ROM_UARTIntStatus(GET_UART_BASE(uart_config[uart].dev), true);
    ROM_UARTIntClear(GET_UART_BASE(uart_config[uart].dev), ulStatus);

    /* Are we interrupted due to a recieved character */
    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(ROM_UARTCharsAvail(GET_UART_BASE(uart_config[uart].dev)))
        {
            long lchar = ROM_UARTCharGetNonBlocking(GET_UART_BASE(uart_config[uart].dev));
            config[uart].rx_cb(config[uart].arg, (uint8_t)lchar);
        }
    }
    cortexm_isr_end();
}

void UART_0_ISR(void)
{
    irq_handler((uart_t)0);
}

void UART_1_ISR(void)
{
    irq_handler((uart_t)1);
}

void UART_2_ISR(void)
{
    irq_handler((uart_t)2);
}
