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

#ifndef CONFIG_BOARD_HAS_HXTAL
#define CONFIG_BOARD_HAS_HXTAL  1   /**< The board provides a high frequency oscillator. */
#endif

#ifndef CONFIG_BOARD_HAS_LXTAL
#define CONFIG_BOARD_HAS_LXTAL  1   /**< The board provides a low frequency oscillator. */
#endif

#ifndef CONFIG_CLOCK_HXTAL
#define CONFIG_CLOCK_HXTAL      MHZ(8)      /**< HXTAL frequency */
#endif

#include "periph_cpu.h"
#include "periph_common_conf.h"

#include "cfg_i2c_default.h"
#include "cfg_timer_default.h"
#include "cfg_uart_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIMER2,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B, 0), .cc_chan = 2 },
                        { .pin = GPIO_PIN(PORT_B, 1), .cc_chan = 3 },
                        /* unused channels have to be defined by GPIO_UNDEF */
                        { .pin = GPIO_UNDEF, .cc_chan = 0 },
                        { .pin = GPIO_UNDEF, .cc_chan = 1 },
                    },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1,
    },
#if !defined(MODULE_PERIPH_CAN)
    {
        .dev      = TIMER3,
        .rcu_mask = RCU_APB1EN_TIMER3EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B, 8), .cc_chan = 2 },
                        { .pin = GPIO_PIN(PORT_B, 9), .cc_chan = 3 },
                        /* unused channels have to be defined by GPIO_UNDEF */
                        { .pin = GPIO_UNDEF, .cc_chan = 0 },
                        { .pin = GPIO_UNDEF, .cc_chan = 1 },
                    },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1,
    },
#endif
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
