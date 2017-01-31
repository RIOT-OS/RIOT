/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_avsextrem
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the avsextrem
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "lpc2387.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (72000000U)         /* this board runs with 72MHz */

#define CLOCK_PCLK          (CLOCK_CORECLOCK)
/** @} */

/**
 * @name   Timer configuration, select a number from 1 to 4
 * @{
 */
#define TIMER_NUMOF         (1U)
/** @} */

/**
 * @name Real Time Clock configuration
 */
#define RTC_NUMOF           (1U)
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           (1)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
