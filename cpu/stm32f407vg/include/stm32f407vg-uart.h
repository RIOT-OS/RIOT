/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file subject to the terms and conditions of the GNU Lesser General Public
License. See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#ifndef STM32F407VG_UART_H
#define STM32F407VG_UART_H

/**
 * @defgroup	stm32f407vg_uart		stm32f407vg uart
 * @ingroup		stm32f407vg
 *
 *
 * @{
 */

/**
 * @file
 * @brief		stm32f407vg uart
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @version     $Revision: 1998 $
 *
 */
#include "stm32f4xx.h"

void uart_init(USART_TypeDef*);
int uart_readc(void);
void uart_writec(char);
/** @} */
#endif /* end STM32F407VG_UART_H */
