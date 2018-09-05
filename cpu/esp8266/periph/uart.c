/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include "common.h"

#include "cpu.h"
#include "irq_arch.h"
#include "log.h"
#include "sched.h"
#include "thread.h"

#include "periph/uart.h"

#include "eagle_soc.h"
#include "esp/uart_regs.h"
#include "sdk/sdk.h"

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static uint8_t IRAM __uart_rx_one_char (uart_t uart);
static void __uart_tx_one_char(uart_t uart, uint8_t data);
static void __uart_intr_enable (uart_t uart);
static void IRAM __uart_intr_handler (void *para);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    CHECK_PARAM_RET (uart < UART_NUMOF, -1);

    DEBUG("%s uart=%d, rate=%d, rx_cb=%p, arg=%p\n", __func__, uart, baudrate, rx_cb, arg);

    /* setup the baudrate */
    uart_div_modify(uart, UART_CLK_FREQ / baudrate);

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    if (rx_cb) {
        ets_isr_attach (ETS_UART_INUM, __uart_intr_handler, 0);

        /* since reading is done byte for byte we set the RX FIFO FULL */
        /* interrupt level to 1 byte */
        UART(uart).CONF1 = SET_FIELD(UART(uart).CONF1, UART_CONF1_RXFIFO_FULL_THRESHOLD, 1);

        /* enable the RX FIFO FULL interrupt */
        __uart_intr_enable (uart);
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    CHECK_PARAM (uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        __uart_tx_one_char(uart, data[i]);
    }
}

void uart_poweron (uart_t uart)
{
    /* UART can't be powered on/off, just return */
}

void uart_poweroff (uart_t uart)
{
    /* UART can't be powered on/off, just return */
}

void IRAM __uart_intr_handler (void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter ();

    DEBUG("%s \n", __func__);

    /*
     * UART0 and UART1 interrupts are combined togther. So we have to
     * iterate over all UART devices and test the INT_STATUS register for
     * interrupts
     */
    for (int i = 0; i < UART_NUMOF; i++) {
        uart_t uart = UART_DEV(0); /* UartDev.buff_uart_no; */
        if (UART(uart).INT_STATUS & UART_INT_STATUS_RXFIFO_FULL) {
            /* clear interrupt flag */
            uint8_t data = __uart_rx_one_char (uart);

            /* call registered RX callback function */
            isr_ctx[uart].rx_cb(isr_ctx[uart].arg, data);

            /* clear interrupt flag */
            UART(uart).INT_CLEAR |= UART_INT_CLEAR_RXFIFO_FULL;
        }
        else {
            /* TODO handle other type of interrupts, for the moment just clear them */
            UART(uart).INT_CLEAR = 0x1f;
        }
    }
    irq_isr_exit ();
}

/* RX/TX FIFO capacity is 128 byte */
#define UART_FIFO_MAX 127

/* receive one data byte with wait */
static uint8_t IRAM __uart_rx_one_char (uart_t uart)
{
    /* uint8_t fifo_len = FIELD2VAL(UART_STATUS_RXFIFO_COUNT, UART(uart).STATUS); */

    /* wait until at least von byte is in RX FIFO */
    while (!FIELD2VAL(UART_STATUS_RXFIFO_COUNT, UART(uart).STATUS)) {}

    /* read the lowest byte from RX FIFO register */
    return UART(uart).FIFO & 0xff; /* only bit 0 ... 7 */
}

/* send one data byte with wait */
static void __uart_tx_one_char(uart_t uart, uint8_t data)
{
    /* wait until at least one byte is avaiable in the TX FIFO */
    while (FIELD2VAL(UART_STATUS_TXFIFO_COUNT, UART(uart).STATUS) >= UART_FIFO_MAX) {}

    /* send the byte by placing it in the TX FIFO */
    UART(uart).FIFO = data;
}

static void __uart_intr_enable(uart_t uart)
{
    UART(uart).INT_ENABLE |= UART_INT_ENABLE_RXFIFO_FULL;
    ETS_INTR_ENABLE(ETS_UART_INUM);

    DEBUG("%s %08x\n", __func__, UART(uart).INT_ENABLE);
}

void uart_print_config(void)
{
    LOG_INFO("\tUART_DEV(0): txd=%d rxd=%d\n", UART0_TXD, UART0_RXD);
}
