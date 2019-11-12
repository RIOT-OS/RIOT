/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       MSB-A2 peripheral configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define XTAL_HZ             (16000000U)         /* the board provides a 16 MHz XTAL */

#define CLOCK_CORECLOCK     (72000000U)         /* the msba2 runs with 72MHz */

#define CLOCK_PCLK          (CLOCK_CORECLOCK)
/** @} */

/**
 * @name    Timer configuration, select a number from 1 to 4
 * @{
 */
#define TIMER_NUMOF         (1U)
/** @} */

/**
 * @name    PWM device and pinout configuration
 *
 * Currently, we only support a single device and 3 channels, the implementation
 * is fixed on PWM1.
 * @{
 */
#define PWM_NUMOF           (1U)

/* PWM_0 device configuration */
#define PWM_CHANNELS      (3)
#define PWM_CH0           (3)
#define PWM_CH0_MR        PWM1MR3
#define PWM_CH1           (4)
#define PWM_CH1_MR        PWM1MR4
#define PWM_CH2           (5)
#define PWM_CH2_MR        PWM1MR5
/* PWM_0 pin configuration */
#define PWM_PORT          PINSEL4
#define PWM_CH0_PIN       (2)
#define PWM_CH1_PIN       (3)
#define PWM_CH2_PIN       (4)
#define PWM_FUNC          (1)
/** @} */


/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .irq_prio_rx = 6,
        .pinsel_rx   = 0,
        .pinsel_tx   = 0,
        .pinsel_msk_rx = BIT4,
        .pinsel_msk_tx = BIT6,
    },
    {
        .dev = UART1,
        .irq_prio_rx = 6,
        .pinsel_rx   = 4,
        .pinsel_tx   = 4,
        .pinsel_msk_rx = BIT3,
        .pinsel_msk_tx = BIT1,
    },
    {
        .dev = UART2,
        .irq_prio_rx = 6,
        .pinsel_rx   = 0,
        .pinsel_tx   = 0,
        .pinsel_msk_rx = BIT22,
        .pinsel_msk_tx = BIT20,
    },
    {
        .dev = UART3,
        .irq_prio_rx = 6,
        .pinsel_rx   = 9,
        .pinsel_tx   = 9,
        .pinsel_msk_rx = BIT26 | BIT27,
        .pinsel_msk_tx = BIT24 | BIT25,
    },
};

#define UART_NUMOF          (4)
/** @} */

/**
 * @name    SPI configuration
 *
 * The SPI implementation is very much fixed, so we don't need to configure
 * anything besides the mandatory SPI_NUMOF.
 * @{
 */
#define SPI_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
