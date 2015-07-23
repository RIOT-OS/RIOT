/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_zedboard
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the zedboard
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           0
#define UART_1_EN           1
#define UART_0_DEV         UART0
#define UART_1_DEV         UART1
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1
#define TIMER_3_EN          0
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (0U)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (0U)
#define SPI_0_EN            0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
