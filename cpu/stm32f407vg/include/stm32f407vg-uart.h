/**
 *
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */

#ifndef STM32F407VG_UART_H
#define STM32F407VG_UART_H

#include "stm32f4xx.h"

void uart_init(USART_TypeDef*);
int uart_readc(void);
void uart_writec(char);
/** @} */
#endif /* end STM32F407VG_UART_H */
