/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard the file in case no UART is defined */
#if (UART_0_EN)

/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    uart_rx_cb_t rx_cb;         /**< receive callback */
    uart_tx_cb_t tx_cb;         /**< transmit callback */
    void *arg;                  /**< callback argument */
} uart_conf_t;

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

// The list of possible interrupts for the console UART.
//
//*****************************************************************************
static const unsigned long g_ulUARTInt[3] =
{
    INT_UART0, 
	INT_UART1, 
	INT_UART2
};

int uart_init_testing(uart_t uart, uint32_t baudrate)
{
	// Enable lazy stacking for interrupt handlers. This allows floating point instructions to be
	// used within interrupt handers, but at the expense of extra stack usuage.
	const unsigned long srcClock = SysCtlClockGet();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTDisable(UART0_BASE);
	UARTConfigSetExpClk(UART0_BASE,srcClock, baudrate,
			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
			 UART_CONFIG_WLEN_8));

	//Enable the UART interrupt
	UARTEnable(UART0_BASE);

    // Prompt for text to be entered.
    //
    //UARTSend((unsigned char *)"\033[2JEnter text: ", 16);

    //
    // Loop forever echoing data through the UART.
    //
	printf("Passed Testing\n");
	return 1;
}

void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}




/**********************************************************************************/
/* Configuring the UART console
 */
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
	// The base address of the Choosen UART
	//	unsigned long ulBase=0;

	// Check the arguments
	ASSERT(uart == 0);

	// Check to make sure the UART peripheral is present
	if(!SysCtlPeripheralPresent(SYSCTL_PERIPH_UART0))
	{
		return -1;
	}

	// Select the base address of the UART
//	ulBase = g_ulUARTBase[uart];

	// Configure the relevant UART pins for operations as a UART rather than GPIOs.
#if UART_0_EN
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Set GPIO A0 and A1 as UART pins
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
#endif
	UARTDisable(UART0_BASE);
	// Enable the UART Peripheral for use
	//	SysCtlPeripheralEnable(g_ulUARTPeriph[uart]);
	// Configure the UART
	
	/* save callbacks */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(), baudrate,
			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
			 UART_CONFIG_WLEN_8));



	// Enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	IntEnable(INT_UART0);

	UARTEnable(UART0_BASE);

	return 1;
}
	
	
	
int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(), baudrate,
			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
			 UART_CONFIG_WLEN_8));


	// Enable the UART interrupt
	//UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	//IntEnable(INT_UART0);

	UARTEnable(UART0_BASE);

    return 1;
}

void uart_tx_begin(uart_t uart)
{
	// enable TX interrupt
	UARTIntEnable(UART0_BASE, UART_INT_TX);
}

int uart_write(uart_t uart, char data)
{
	UARTCharPutNonBlocking(UART0_BASE, data);
    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
	*data = (char)UARTCharGet(UART0_BASE);
	return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
	UARTCharPut(UART0_BASE, data);
    return 1;
}

void uart_poweron(uart_t uart)
{
	UARTEnable(UART0_BASE);
}

void uart_poweroff(uart_t uart)
{
	UARTDisable(UART0_BASE);
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************

void UARTIntHandler(void)
{
	unsigned long ulStatus;
	char cChar;
	long lChar;

	// Get the interrupt status
	ulStatus = UARTIntStatus(UART0_BASE, true);

	// Clear the asserted interrupts
	UARTIntClear(UART0_BASE, ulStatus);

	// Are we interrupted due to TX done
	if(ulStatus & UART_INT_TX)
	{
		// Turn off the Transmit Interrupt
		if (config[UART_0].tx_cb(config[UART_0].arg) == 0){
			UARTIntDisable(UART0_BASE, UART_INT_TX);
		}
	}

	// Are we interrupted due to a recieved character
	if(ulStatus & (UART_INT_RX | UART_INT_RT))
	{
		// Get all the available characters from the UART
		while(UARTCharsAvail(UART0_BASE))
		{
			// Read a character
			lChar = UARTCharGetNonBlocking(UART0_BASE);
			cChar = (unsigned char)(lChar & 0xFF);
			config[UART_0].rx_cb(config[UART_0].arg, cChar);
		}
	}
	if (sched_context_switch_request) {
        thread_yield();
    }
}




#endif /* (UART_0_EN || UART_1_EN) */
