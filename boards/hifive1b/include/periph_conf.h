/*
 * Copyright (C) 2019 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1b RISC-V board
 *
 * @author      Ken Rabold
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core Clock configuration
 * @{
 */
/* As defined in boards/hifive1/board.c CPU_DESIRED_FREQ **/
#define CLOCK_CORECLOCK             (200000000ul)
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (32)
#define XTIMER_HZ                   (32768ul)
/** @} */

/**
 * @name    Timer configuration
 *
 * @{
 */
#define TIMER_NUMOF                 (1)
/** @} */

/**
 * @name    RTT/RTC configuration
 *
 * @{
 */
#define RTT_FREQUENCY               (1)             /* in Hz */
#define RTT_MAX_VALUE               (0xFFFFFFFF)
#define RTT_INTR_PRIORITY           (2)

/** @} */

/**
 * @name    GPIO configuration
 *
 * @{
 */
#define GPIO_INTR_PRIORITY          (3)
/** @} */

/**
 * @name    PWM configuration
 *
 * @{
 */
#define PWM_NUMOF                   (3)
/** @} */

/**
 * @name    UART configuration
 *
 * @{
 */
#define UART_NUMOF                  (2)
#define UART0_RX_INTR_PRIORITY      (2)
#define UART1_RX_INTR_PRIORITY      (2)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
