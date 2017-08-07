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
 * @author      Attilio Dona'
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 *
 */

#include "cc3200.h"

#define CLOCK_CORECLOCK     (80000000U)      /* desired core clock frequency */

#define XTIMER_BACKOFF  9
#define XTIMER_OVERHEAD 5

#define XTIMER_SHIFT 0
#define XTIMER_USEC_TO_TICKS_FACTOR 80

#define XTIMER_USEC_TO_TICKS(value) ( value*XTIMER_USEC_TO_TICKS_FACTOR )
#define XTIMER_TICKS_TO_USEC(value) ( value/XTIMER_USEC_TO_TICKS_FACTOR )




// CC3200 has 4 timer blocks
#define TIMER_0_EN 1
#define TIMER_1_EN 1
#define TIMER_2_EN 1
#define TIMER_3_EN 1


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


/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1
#define SPI_IRQ_PRIO        1

/* SPI_0 device configuration */
//#define SPI_0_DEV           GSPI_BASE
#define SPI_0_PIN_SCK       7         // PIN_05
#define SPI_0_PIN_MISO      14        // PIN_06
#define SPI_0_PIN_MOSI      15        // PIN_07
#define SPI_0_PIN_CS        18        // PIN_08

/** @} */

#include "board.h"


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
