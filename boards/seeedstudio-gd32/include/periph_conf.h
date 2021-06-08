/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedstudio-gd32
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SeeedStudio GD32 RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This board provides an high frequency oscillator */
#ifndef CONFIG_BOARD_HAS_HXTAL
#define CONFIG_BOARD_HAS_HXTAL        1
#endif

#define CLOCK_HXTAL         MHZ(8)             /**< HXTAL frequency */
#define CLOCK_CORECLOCK     MHZ(104)           /**< CPU clock frequency in Hz */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIMER2,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER2_IRQn
    },
    {
        .dev      = TIMER3,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER3EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER3_IRQn
    }
};

#define TIMER_0_IRQN        TIMER2_IRQn
#define TIMER_1_IRQN        TIMER3_IRQn

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART0,
        .rcu_mask   = RCU_APB2EN_USART0EN_Msk,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .bus        = APB2,
        .irqn       = USART0_IRQn,
    },
};

#define UART_0_IRQN         USART0_IRQn

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
