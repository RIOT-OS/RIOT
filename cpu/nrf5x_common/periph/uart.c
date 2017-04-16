/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral UART interface
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#ifdef CPU_MODEL_NRF52840XXAA
#define PSEL_RXD         NRF_UART0->PSEL.RXD
#define PSEL_TXD         NRF_UART0->PSEL.TXD
#define PSEL_RTS         NRF_UART0->PSEL.RTS
#define PSEL_CTS         NRF_UART0->PSEL.CTS
#else
#define PSEL_RXD         NRF_UART0->PSELRXD
#define PSEL_TXD         NRF_UART0->PSELTXD
#define PSEL_RTS         NRF_UART0->PSELRTS
#define PSEL_CTS         NRF_UART0->PSELCTS
#endif

/**
 * @brief Allocate memory for the interrupt context
 */
static uart_isr_ctx_t uart_config;

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart != 0) {
        return UART_NODEV;
    }

    /* remember callback addresses and argument */
    uart_config.rx_cb = rx_cb;
    uart_config.arg = arg;

#ifdef CPU_FAM_NRF51
   /* power on the UART device */
    NRF_UART0->POWER = 1;
#endif

    /* reset configuration registers */
    NRF_UART0->CONFIG = 0;

    /* configure RX pin */
    if (rx_cb) {
        gpio_init(UART_PIN_RX, GPIO_IN);
        PSEL_RXD = UART_PIN_RX;
    }

    /* configure TX pin */
    gpio_init(UART_PIN_TX, GPIO_OUT);
    PSEL_TXD = UART_PIN_TX;

    /* enable HW-flow control if defined */
#if UART_HWFLOWCTRL
    /* set pin mode for RTS and CTS pins */
    gpio_init(UART_PIN_RTS, GPIO_OUT);
    gpio_init(UART_PIN_CTS, GPIO_IN);
    /* configure RTS and CTS pins to use */
    PSEL_RTS = UART_PIN_RTS;
    PSEL_CTS = UART_PIN_CTS;
    NRF_UART0->CONFIG |= UART_CONFIG_HWFC_Msk;  /* enable HW flow control */
#else
    PSEL_RTS = 0xffffffff;            /* pin disconnected */
    PSEL_CTS = 0xffffffff;            /* pin disconnected */
#endif

    /* select baudrate */
    switch (baudrate) {
        case 1200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1200;
            break;
        case 2400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud2400;
            break;
        case 4800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud4800;
            break;
        case 9600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud9600;
            break;
        case 14400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud14400;
            break;
        case 19200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud19200;
            break;
        case 28800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud28800;
            break;
        case 38400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud38400;
            break;
        case 57600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud57600;
            break;
        case 76800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud76800;
            break;
        case 115200:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
            break;
        case 230400:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud230400;
            break;
        case 250000:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud250000;
            break;
        case 460800:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud460800;
            break;
        case 921600:
            NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud921600;
            break;
        default:
            return UART_NOBAUD;
    }

    /* enable the UART device */
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
    /* enable TX and RX */
    NRF_UART0->TASKS_STARTTX = 1;

    if (rx_cb) {
        NRF_UART0->TASKS_STARTRX = 1;
        /* enable global and receiving interrupt */
        NVIC_EnableIRQ(UART_IRQN);
        NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if (uart == 0) {
        for (size_t i = 0; i < len; i++) {
            /* This section of the function is not thread safe:
                - another thread may mess up with the uart at the same time.
               In order to avoid an infinite loop in the interrupted thread,
               the TXRDY flag must be cleared before writing the data to be
               sent and not after. This way, the higher priority thread will
               exit this function with the TXRDY flag set, then the interrupted
               thread may have not transmitted his data but will still exit the
               while loop.
            */

            /* reset ready flag */
            NRF_UART0->EVENTS_TXDRDY = 0;
            /* write data into transmit register */
            NRF_UART0->TXD = data[i];
            /* wait for any transmission to be done */
            while (NRF_UART0->EVENTS_TXDRDY == 0) {}
        }
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->TASKS_STARTTX = 1;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
    NRF_UART0->TASKS_SUSPEND;
}

void isr_uart0(void)
{
    if (NRF_UART0->EVENTS_RXDRDY == 1) {
        NRF_UART0->EVENTS_RXDRDY = 0;
        uint8_t byte = (uint8_t)(NRF_UART0->RXD & 0xff);
        uart_config.rx_cb(uart_config.arg, byte);
    }
    cortexm_isr_end();
}
