/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *               2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-gd32
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SeeedStudio GD32 RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

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

#ifndef SPI_DEV_1_USED
#define SPI_DEV_1_USED          1   /**< Enable SPI_DEV(1) by default for the connected Flash */
#endif

#ifndef I2C_DEV_1_USED
#define I2C_DEV_1_USED          1   /**< Enable I2C_DEV(1) by default */
#endif

#include "periph_cpu.h"
#include "periph_common_conf.h"

#include "cfg_i2c_default.h"
#include "cfg_spi_default.h"
#include "cfg_timer_default.h"
#include "cfg_uart_default.h"
#include "cfg_usbdev_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 2), .dev = 0, .chan = 2 },
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan = 3 },
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 0, .chan = 10 },
    { .pin = GPIO_PIN(PORT_C, 1), .dev = 0, .chan = 11 },
    { .pin = GPIO_PIN(PORT_C, 2), .dev = 0, .chan = 12 },
    { .pin = GPIO_PIN(PORT_C, 3), .dev = 0, .chan = 13 },
    { .pin = GPIO_PIN(PORT_C, 4), .dev = 0, .chan = 14 },
    { .pin = GPIO_PIN(PORT_C, 5), .dev = 0, .chan = 15 },
    /* ADC Temperature channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 16 },
    /* ADC VREF channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 17 },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

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

/** @} */
