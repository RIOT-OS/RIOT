/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc3200
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the CC3200 board
 *
 * @author      Attilio Dona' <@attiliodona>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 * @{
 */

#include "cc3200.h"

#define CLOCK_CORECLOCK     (80000000U)      /* desired core clock frequency */

// CC3200 has 4 timer blocks
#define TIMER_0_EN 1
#define TIMER_1_EN 1
#define TIMER_2_EN 1
#define TIMER_3_EN 1


#define SEC_TO_TICKS(sec)   80000000*sec
#define MSEC_TO_TICKS(msec) 80000*msec
#define USEC_TO_TICKS(usec) 80*usec


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          1

#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0

#define UART_IRQ_PRIO       INT_PRIORITY_LVL_3

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
