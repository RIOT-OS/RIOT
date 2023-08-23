/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32vf103v-eval
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for GD32VF103V-EVAL board
 *
 * @author      Jason Parker <jason.parker@bissell.com>
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
    { .pin = GPIO_PIN(PORT_C, 3), .dev = 0, .chan = 13 },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    { .pin = GPIO_PIN(PORT_A, 4), .chan = 0 },
    { .pin = GPIO_PIN(PORT_A, 5), .chan = 1 },
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
//        .dev      = TIMER2,
//        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
//        .chan     = {
//                        { .pin = GPIO_PIN(PORT_B, 0), .cc_chan = 2 },
//                        { .pin = GPIO_PIN(PORT_B, 1), .cc_chan = 3 },
//                        /* unused channels have to be defined by GPIO_UNDEF */
//                        { .pin = GPIO_UNDEF, .cc_chan = 0 },
//                        { .pin = GPIO_UNDEF, .cc_chan = 1 },
//                    },
//        .af       = GPIO_AF_OUT_PP,
//        .bus      = APB1,
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
