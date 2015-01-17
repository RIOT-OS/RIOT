/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_mchck
 * @{
 *
 * @file
 * @name       Peripheral MCU configuration for the mchck board
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration.
 * @{
 */
#define CLOCK_CORECLOCK     (48000000U)         /* desired core clock frequency. currently unused. */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          3
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_IRQ_PRIO       1
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
