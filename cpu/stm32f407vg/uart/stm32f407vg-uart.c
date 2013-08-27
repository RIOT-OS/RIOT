/******************************************************************************
 Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

 These sources were developed at the Freie Universitaet Berlin, Computer Systems
 and Telematics group (http://cst.mi.fu-berlin.de).
 -------------------------------------------------------------------------------
 This file is part of FeuerWare.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 FeuerWare is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see http://www.gnu.org/licenses/ .
 --------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://scatterweb.mi.fu-berlin.de
 and the mailinglist (subscription via web site)
 scatterweb@lists.spline.inf.fu-berlin.de
 *******************************************************************************/

/**
 * @file
 * @ingroup
 * @brief
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author
 * @version
 *
 * @note
 */

#include <stdint.h>
#include <ringbuffer.h>
#include <mutex.h>
#include <stdbool.h>
#include <sched.h>

#include "stm32f4xx_usart.h"
#include "cpu.h"

#define UART_BUFSIZE 255
static ringbuffer_t uart_rb;
static char uart_buffer[UART_BUFSIZE];
static char uart_waiting = 0;

static mutex_t uart_mutex;
static USART_TypeDef* active_uart;

void uart_enable_peripherals(USART_TypeDef* uart) {
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	USART_Cmd(uart, DISABLE); // disable USARTx

	switch ((uint32_t) uart) {
		case (uint32_t)USART2:

			/* enable peripheral clock for USART2 */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

			/* GPIOA clock enable */
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

			/* GPIOA Configuration:  USART2 TX on PA2 */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);

			/* Connect USART2 pins to AF2 */
			// TX = PA2
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // TX
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // RX

			USART_InitStructure.USART_BaudRate = 115200;
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
			USART_InitStructure.USART_StopBits = USART_StopBits_1;
			USART_InitStructure.USART_Parity = USART_Parity_No;
			USART_InitStructure.USART_HardwareFlowControl =
					USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx );

			// init the interrupt
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // we want to configure the USART1 interrupts
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F; // this sets the priority group of the USART1 interrupts
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F; // this sets the subpriority inside the group
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART1 interrupts are globally enabled
			break;

		case (uint32_t)USART6:
			/* enable peripheral clock for USART2 */
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
			/* GPIOC clock enable */
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

			/* GPIOA Configuration:  USART6 TX,RX on PC06,PC07 */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(GPIOC, &GPIO_InitStructure);

			/* Connect USART6 pins to AF2 */
			// TX = PC06, RX = PC07
			GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
			GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

			USART_InitStructure.USART_BaudRate = 115200;
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
			USART_InitStructure.USART_StopBits = USART_StopBits_1;
			USART_InitStructure.USART_Parity = USART_Parity_No;
			USART_InitStructure.USART_HardwareFlowControl =
					USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx );

			// init the interrupt
			NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn; // we want to configure the USART1 interrupts
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F; // this sets the priority group of the USART1 interrupts
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F; // this sets the subpriority inside the group
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART1 interrupts are globally enabled
			break;
	}

	USART_Init(uart, &USART_InitStructure);
	NVIC_Init(&NVIC_InitStructure);	// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	USART_ITConfig(uart, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt

	USART_Cmd(uart, ENABLE); // enable USARTx

	/* Loop until uart ready */
	while (USART_GetFlagStatus(active_uart, USART_FLAG_TC ) == RESET) {
	}
}

void uart_init(USART_TypeDef* uart) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	active_uart = uart;

	uart_enable_peripherals(active_uart);

	// buffering initialization
	dINT(); //unsigned primask = disableIRQ();
	mutex_init(&uart_mutex);
	mutex_lock(&uart_mutex);
	ringbuffer_init(&uart_rb, uart_buffer, UART_BUFSIZE);
	eINT(); //restoreIRQ(primask);

}

static void uart_interrupt(void) {
	// check if the USART1 receive interrupt flag was set
	if (USART_GetITStatus(active_uart, USART_IT_RXNE )) {

		static uint8_t cnt = 0; // this counter is used to determine the string length
		char c = active_uart->DR; // the character from the USART1 data register is saved in t

		rb_add_element(&uart_rb, c);

		if (uart_waiting) {
			uart_waiting = 0;
			mutex_unlock(&uart_mutex);
		}
	}

	if (sched_context_switch_request) {
		thread_yield(); // trigger PendSV for context switch
	}
}

// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
__attribute__((naked))
void USART6_IRQHandler(void) {

	interrupt_entry();

	uart_interrupt();

	interrupt_return();
}

// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
__attribute__((naked))
void USART2_IRQHandler(void) {

	interrupt_entry();

	uart_interrupt();

	interrupt_return();
}


int uart_readc(void) {
	//unsigned primask = disableIRQ();

	if (uart_rb.avail == 0) {
		uart_waiting++;
		mutex_lock(&uart_mutex);
	}

	int c = rb_get_element(&uart_rb);

	//restoreIRQ(primask);

	return c;
}

void uart_writec(char c) {
	//unsigned primask = disableIRQ();

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(active_uart, USART_FLAG_TC ) == RESET) {;}
	USART_SendData(active_uart, c);

}
