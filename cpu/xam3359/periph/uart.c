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
 * @author      Martin Kessel <martin.kessel@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/uart.h"
#include "periph_conf.h"

#include "uart_irda_cir.h"

#include "beaglebone.h"
#include "consoleUtils.h" 

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define BAUD_RATE_115200          (115200)
#define UART_MODULE_INPUT_CLK     (48000000)


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
//static uart_conf_t config[UART_NUMOF];


int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{  
	return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
	/* Configuring the system clocks for UART0 instance. */
	UART0ModuleClkConfig();

	/* Performing the Pin Multiplexing for UART0 instance. */
	UARTPinMuxSetup(0);

	/* Performing a module reset. */
	UARTModuleReset(SOC_UART_0_REGS);

	/* Performing Baud Rate settings. */
	unsigned int divisorValue = 0;

	  /* Computing the Divisor Value. */
	divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
					      BAUD_RATE_115200,
					      UART16x_OPER_MODE,
					      UART_MIR_OVERSAMPLING_RATE_42);

	  /* Programming the Divisor Latches. */
	UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);
	  
	  

	/* Switching to Configuration Mode B. */
	UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);

	/* Programming the Line Characteristics. */
	UARTLineCharacConfig(SOC_UART_0_REGS, 
			    (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1), 
			    UART_PARITY_NONE);

	/* Disabling write access to Divisor Latches. */
	UARTDivisorLatchDisable(SOC_UART_0_REGS);

	/* Disabling Break Control. */
	UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);

	/* Switching to UART16x operating mode. */
	UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);

	/* Select the console type based on compile time check */
	ConsoleUtilsSetType(CONSOLE_UART);
    
    	return 0;
}

void uart_tx_begin(uart_t uart)
{

}

void uart_tx_end(uart_t uart)
{

}

int uart_write(uart_t uart, char data)
{
	UARTCharPutNonBlocking(SOC_UART_0_REGS,data);
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
	*data =  UARTCharGet(SOC_UART_0_REGS);
    return 1;  
}

int uart_write_blocking(uart_t uart, char data)
{
	UARTCharPut(SOC_UART_0_REGS, data);   
    return 1;
}
