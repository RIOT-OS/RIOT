/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32vf103c_start
 * @{
 *
 * @file
 * @brief       Board specific definitions for the GD32VF103C-START board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
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

#ifndef I2C_DEV_1_USED
#define I2C_DEV_1_USED          1   /**< Enable I2C_DEV(1) by default */
#endif

#include "periph_cpu.h"
#include "periph_common_conf.h"

#include "cfg_spi_default.h"
#include "cfg_uart_default.h"
#include "cfg_usbdev_default.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * @name    I2C configuration
 *
 * @note    This board may require external pullup resistors for i2c operation.
 * @{
 */

/**
 * @brief   PB10/PB11 (D15/D14) as I2C for Arduino UNO compatibility
 *
 * PB10/PB11 are located where Arduino UNO shields expect I2C.
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .rcu_mask       = RCU_APB1EN_I2C1EN_Msk,
        .irqn           = I2C1_EV_IRQn,
    }
};

#define I2C_NUMOF   ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 2), .dev = 0, .chan = 2 },
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan = 3 },
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 0, .chan = 4 },
    { .pin = GPIO_PIN(PORT_A, 5), .dev = 0, .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .dev = 0, .chan = 6 },
    /* ADC Temperature channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 16 },
    /* ADC VREF channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 17 },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIMER0,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .bus      = APB2,
        .irqn     = TIMER0_Channel_IRQn
    },
    {
        .dev      = TIMER1,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER1EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER1_IRQn
    },
    /* TIMER2 and TIMER3 are used for PWM pins */
    {
        .dev      = TIMER4,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER4EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER4_IRQn
    }
};

#define TIMER_0_IRQN        TIMER0_Channel_IRQn
#define TIMER_1_IRQN        TIMER1_IRQn
#define TIMER_2_IRQN        TIMER4_IRQn

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
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
                        { .pin = GPIO_PIN(PORT_B, 5), .cc_chan = 1 },
                        { .pin = GPIO_PIN(PORT_B, 0), .cc_chan = 2 },
                        /* unused channels have to be defined by GPIO_UNDEF */
                        { .pin = GPIO_UNDEF, .cc_chan = 0 },
                        { .pin = GPIO_UNDEF, .cc_chan = 3 },
                    },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1,
    },
    {
        .dev      = TIMER3,
        .rcu_mask = RCU_APB1EN_TIMER3EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B, 7), .cc_chan = 1 },
                        { .pin = GPIO_PIN(PORT_B, 8), .cc_chan = 2 },
                        /* unused channels have to be defined by GPIO_UNDEF */
                        { .pin = GPIO_UNDEF, .cc_chan = 0 },
                        { .pin = GPIO_UNDEF, .cc_chan = 3 },
                    },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1,
    },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
