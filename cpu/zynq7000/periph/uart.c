/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "board.h"
#include "zynq_interrupts.h"

#define RX_FIFO_EMPTY             0x02
#define TX_FIFO_FULL              0x10

#define IXR_TX_EMPTY	0x8
#define IXR_RXOVR		0x1

#if UART_0_EN
void isr_uart0(void *arg);
#endif
#if UART_1_EN
void isr_uart1(void *arg);
#endif

/* guard file in case no UART device was specified */
#if UART_NUMOF

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
	uart_rx_cb_t rx_cb;
	uart_tx_cb_t tx_cb;
	void *arg;
} uart_conf_t;

/**
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 */
static inline void irq_handler(uart_t uartnum, volatile xuart_ps * dev);

/**
 * @brief Data structure holding the callbacks and argument for each UART device
 */
static uart_conf_t uart_config[UART_NUMOF];

/**
 * @brief Allocate memory to store the callback functions.
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb,
		uart_tx_cb_t tx_cb, void *arg) {

	/* initialize UART in blocking mode fist */
	int res = uart_init_blocking(uart, baudrate);
	if (res != 0) {
		return res;
	}

	/* remember callback addresses and argument */
	uart_config[uart].rx_cb = rx_cb;
	uart_config[uart].tx_cb = tx_cb;
	uart_config[uart].arg = arg;

	/* configure interrupts and enable RX interrupt */
	switch (uart) {
#if UART_0_EN
	case UART_0:
		register_interrupt_handler(UART0_INT_ID, INT_HIGH_LEVEL, &isr_uart0, NULL);
		UART_0_DEV->chnl_int_sts_reg0 = 0xFFFFFFFF;
		UART_0_DEV->rcvr_fifo_trigger_level0 = 1;
		UART_0_DEV->intpt_en_reg0 = IXR_RXOVR;
		break;
#endif
#if UART_1_EN
	case UART_1:
		register_interrupt_handler(UART1_INT_ID, INT_HIGH_LEVEL, &isr_uart1, NULL);
		UART_1_DEV->chnl_int_sts_reg0 = 0xFFFFFFFF;
		UART_1_DEV->rcvr_fifo_trigger_level0 = 1;
		UART_1_DEV->intpt_en_reg0 = IXR_RXOVR;
		break;
#endif
	}
	return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		/* no parity */
		UART_0_DEV->mode_reg0 = 0x20;
		/* reset and enable RX & TX */
		UART_0_DEV->control_reg0 = 0x17;
		break;
#endif
#if UART_1_EN
	case UART_1:
		/* no parity */
		UART_1_DEV->mode_reg0 = 0x20;
		/* reset and enable RX & TX */
		UART_1_DEV->control_reg0 = 0x17;
		break;
#endif
	}
	return 0;
}

void uart_tx_begin(uart_t uart) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		UART_0_DEV->chnl_int_sts_reg0 = IXR_TX_EMPTY;
		UART_0_DEV->intpt_en_reg0 = IXR_TX_EMPTY;
		break;
#endif
#if UART_1_EN
	case UART_1:
		UART_1_DEV->chnl_int_sts_reg0 = IXR_TX_EMPTY;
		UART_1_DEV->intpt_en_reg0 = IXR_TX_EMPTY;
		break;
#endif
	}
}

int uart_write(uart_t uart, char data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		UART_0_DEV->tx_rx_fifo0 = data;
		return 1;
#endif
#if UART_1_EN
	case UART_1:
		UART_1_DEV->tx_rx_fifo0 = data;
		return 1;
#endif
	}
	return 0;
}

int uart_read_blocking(uart_t uart, char *data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		while (UART_0_DEV->channel_sts_reg0 & RX_FIFO_EMPTY);
		*data = UART_0_DEV->tx_rx_fifo0;
		return 1;
#endif
#if UART_1_EN
	case UART_1:
		while (UART_1_DEV->channel_sts_reg0 & RX_FIFO_EMPTY);
		*data = UART_1_DEV->tx_rx_fifo0;
		return 1;
#endif
	}
	return 0;
}

int uart_write_blocking(uart_t uart, char data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		while (UART_0_DEV->channel_sts_reg0 & TX_FIFO_FULL);
		UART_0_DEV->tx_rx_fifo0 = data;
		return 1;
#endif
#if UART_1_EN
	case UART_1:
		while (UART_1_DEV->channel_sts_reg0 & TX_FIFO_FULL);
		UART_1_DEV->tx_rx_fifo0 = data;
		return 1;
#endif
	}
	return 0;
}
#if UART_0_EN
void isr_uart0(void *arg) {
	irq_handler(UART_0, UART_0_DEV);
}
#endif

#if UART_1_EN
void isr_uart1(void *arg) {
	irq_handler(UART_1, UART_1_DEV);
}
#endif

static inline void irq_handler(uart_t uartnum, volatile xuart_ps * dev) {
	uint32_t isr = dev->chnl_int_sts_reg0 & dev->intpt_mask_reg0;

	if (isr & IXR_RXOVR) {
		char data = (char) dev->tx_rx_fifo0;
		uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
		dev->chnl_int_sts_reg0 = IXR_RXOVR;
	}

	if (isr & IXR_TX_EMPTY) {
		if (uart_config[uartnum].tx_cb(uart_config[uartnum].arg) == 0) {
			dev->intpt_dis_reg0 = IXR_TX_EMPTY;
		}
		dev->chnl_int_sts_reg0 = IXR_TX_EMPTY;
	}
}

#endif /* UART_NUMOF */
