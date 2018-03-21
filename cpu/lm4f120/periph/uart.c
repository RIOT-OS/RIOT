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
 * @brief UART device configurations
 */
static uart_isr_ctx_t config[UART_NUMOF];

/**
 * Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* Check the arguments */
    assert(uart < UART_NUMOF);


    /* Check to make sure the UART peripheral is present */
    if(!ROM_SysCtlPeripheralPresent(uart_config[uart].uart_sysctl)){
        return UART_NODEV;
    }

    ROM_SysCtlPeripheralEnable(uart_config[uart].uart_sysctl);
    ROM_SysCtlPeripheralEnable(uart_config[uart].gpio_sysctl);
    //no checks for rx_cb
    ROM_GPIOPinConfigure(uart_config[uart].pins.rx);
    ROM_GPIOPinConfigure(uart_config[uart].pins.tx);
    //no checks for rx_cb
    ROM_GPIOPinTypeUART(uart_config[uart].gpio_port, uart_config[uart].pins.mask);

    ROM_UARTDisable(uart_config[uart].uart_base);
    ROM_UARTConfigSetExpClk(uart_config[uart].uart_base,
            ROM_SysCtlClockGet(), baudrate,
            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
             UART_CONFIG_WLEN_8));

    ROM_UARTEnable(uart_config[uart].uart_base);

/* save callbacks */
    if (rx_cb) {
        config[uart].rx_cb = rx_cb;
        config[uart].arg = arg;
    }

    ROM_UARTTxIntModeSet(uart_config[uart].uart_base,
        uart_config[uart].uart_txint_mode);
      //no checks for rx_cb
    ROM_UARTFIFOLevelSet(uart_config[uart].uart_base,
        uart_config[uart].uart_fifo_tx, uart_config[uart].uart_fifo_rx);
    ROM_UARTFIFOEnable(uart_config[uart].uart_base);

      //no flow control config? maybe add that in

    /* Enable the UART interrupt */
    NVIC_EnableIRQ(uart_config[uart].uart_irq_chan);

    /* Enable RX interrupt */
    (*uart_config[uart].uart_im_r) = (UART_IM_RXIM | UART_IM_RTIM);

    return UART_OK;
}


void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        ROM_UARTCharPut(uart_config[uart].uart_base, (char)data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    ROM_UARTEnable(uart_config[uart].uart_base);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    ROM_UARTDisable(uart_config[uart].uart_base);
}

/**
 * The UART interrupt handler.
 */
static inline void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);

    unsigned long ulStatus;

    ulStatus = ROM_UARTIntStatus(uart_config[uart].uart_base, true);
    ROM_UARTIntClear(uart_config[uart].uart_base, ulStatus);

    /* Are we interrupted due to a recieved character */
    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(ROM_UARTCharsAvail(uart_config[uart].uart_base))
        {
            long lchar = ROM_UARTCharGetNonBlocking(uart_config[uart].uart_base);
            config[uart].rx_cb(config[uart].arg, (uint8_t)lchar);
        }
    }
    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler((uart_t)0);
}
#endif
