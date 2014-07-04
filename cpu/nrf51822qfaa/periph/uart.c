/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        uart.c
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <math.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "nrf51.h"
#include "board.h"
#include "pca10000.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"


/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
	void (*rx_cb)(char);
	void (*tx_cb)(void);
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, void (*rx_cb)(char),
		void (*tx_cb)(void)) { //Manual: nrf51822.pdf page 144

	// initialize basic functionality
	int res = uart_init_blocking(uart, baudrate);
	if (res != 0) {
		return res;
	}

	return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate) {
	int baudrate_real;
	switch (baudrate) {
	case 1200:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud1200;
		break;
	case 2400:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud2400;
		break;
	case 4800:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud4800;
		break;
	case 9600:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud9600;
		break;
	case 14400:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud14400;
		break;
	case 19200:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud19200;
		break;
	case 28800:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud28800;
		break;
	case 38400:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud38400;
		break;
	case 57600:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud57600;
		break;
	case 76800:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud76800;
		break;
	case 115200:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud115200;
		break;
	case 230400:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud230400;
		break;
	case 250000:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud250000;
		break;
	case 460800:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud460800;
		break;
	case 921600:
		baudrate_real = UART_BAUDRATE_BAUDRATE_Baud921600;
		break;

	}
	switch (uart) {
#if UART_0_EN
	case UART_0:
		/* configure PINS */
		// UART_0_PORT->PIO_PDR = UART_0_PINS;
		// UART_0_PORT->PIO_ABSR &= ~UART_0_PINS;
		/** @snippet [Configure UART RX and TX pin] */
		nrf_gpio_cfg_output(RTS_PIN_NUMBER);
		nrf_gpio_cfg_input(CTS_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);

		nrf_gpio_cfg_output(RTS_PIN_NUMBER);
		nrf_gpio_cfg_input(CTS_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);

		NRF_UART0->PSELTXD = TX_PIN_NUMBER;
		NRF_UART0->PSELRXD = RX_PIN_NUMBER;

		//NRF_UART0->PSELCTS = CTS_PIN_NUMBER;
		//NRF_UART0->PSELRTS = RTS_PIN_NUMBER;
		NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);

		NRF_UART0->BAUDRATE = (baudrate_real << UART_BAUDRATE_BAUDRATE_Pos);
		NRF_UART0->ENABLE = (UART_ENABLE_ENABLE_Enabled
				<< UART_ENABLE_ENABLE_Pos);
		NRF_UART0->TASKS_STARTTX = 1;
		NRF_UART0->TASKS_STARTRX = 1;
		NRF_UART0->EVENTS_RXDRDY = 0;

		break;
#endif
	case UART_UNDEFINED:
		return -2;
		break;
	}
	return 0;

}

void uart_tx_begin(uart_t uart) {
	/* TODO */
}

void uart_tx_end(uart_t uart) {
	/* TODO */
}

int uart_write(uart_t uart, char data) {
	switch (uart) {
	case UART_0:
		NRF_UART0->TXD = (uint8_t) data;
		NRF_UART0->EVENTS_TXDRDY = 0;
		break;
	case UART_UNDEFINED:
		return -1;
	}
	return 1;
}

int uart_read_blocking(uart_t uart, char *data) {
	switch (uart) {
	case UART_0:
		while (NRF_UART0->EVENTS_RXDRDY != 1) {
			// Wait for RXD data to be received
		}

		*data = (char) NRF_UART0->RXD;
		NRF_UART0->EVENTS_RXDRDY = 0;

		break;
	case UART_UNDEFINED:
		return -1;
	}
	return 1;
}

int uart_write_blocking(uart_t uart, char data) {
	//Using:
	/* while (ch != '\0')
	 {
	 uart_write_blocking(uart, ch);
	 ch = data[i++];
	 }*/
	switch (uart) {
	case UART_0:
		NRF_UART0->TXD = (uint8_t) data;

		while (NRF_UART0->EVENTS_TXDRDY != 1) {
			// Wait for TXD data to be sent
		}

		NRF_UART0->EVENTS_TXDRDY = 0;

		break;
	case UART_UNDEFINED:
		return -1;
	}
	return 1;

}

