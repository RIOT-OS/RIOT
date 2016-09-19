/*
 * Copyright (C) 2016 University of California, Berkeley
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hamilton
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Hamilton mote
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Michael Andersen <m.andersen@berkeley.edu>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   External oscillator and clock configuration
 *
 * For selection of the used CORECLOCK, we have implemented two choices:
 *
 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
 * - usage of the internal 8MHz oscillator directly, divided by N if needed
 *
 *
 * The PLL option allows for the usage of a wider frequency range and a more
 * stable clock with less jitter. This is why we use this option as default.
 *
 * The target frequency is computed from the PLL multiplier and the PLL divisor.
 * Use the following formula to compute your values:
 *
 * CORECLOCK = ((PLL_MUL + 1) * 1MHz) / PLL_DIV
 *
 * NOTE: The PLL circuit does not run with less than 32MHz while the maximum PLL
 *       frequency is 96MHz. So PLL_MULL must be between 31 and 95!
 *
 *
 * The internal Oscillator used directly can lead to a slightly better power
 * efficiency to the cost of a less stable clock. Use this option when you know
 * what you are doing! The actual core frequency is adjusted as follows:
 *
 * CORECLOCK = 8MHz / DIV
 *
 * NOTE: A core clock frequency below 1MHz is not recommended
 *
 * @{
 */
#define CLOCK_USE_PLL       (1)

#if CLOCK_USE_PLL
/* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#else
/* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC3->COUNT16
#define TIMER_0_CHANNELS    2
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV         TC4->COUNT32
#define TIMER_1_CHANNELS    2
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR         isr_tc4
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
};

#define UART_NUMOF          (0U)
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (PWM_0_EN + PWM_1_EN)
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    2
/* for compatibility with test application */
#define PWM_0_CHANNELS      PWM_MAX_CHANNELS
#define PWM_1_CHANNELS      PWM_MAX_CHANNELS

/* PWM device configuration */
#if PWM_NUMOF
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    {TCC1, {
        /* GPIO pin, MUX value, TCC channel */
        {GPIO_PIN(PA, 6), GPIO_MUX_E, 0},
        {GPIO_PIN(PA, 7), GPIO_MUX_E, 1}
    }},
#endif
#if PWM_1_EN
    {TCC0, {
        /* GPIO pin, MUX value, TCC channel */
        {GPIO_PIN(PA, 18), GPIO_MUX_F, 2},
        {GPIO_PIN(PA, 19), GPIO_MUX_F, 3}
    }},
#endif
};
#endif
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF          (1)
#define SPI_0_EN           1

/*      SPI0 - radio       */
#define SPI_0_DEV          SERCOM4->SPI
#define SPI_IRQ_0          SERCOM4_IRQn
#define SPI_0_DOPO         (1)
#define SPI_0_DIPO         (0)

#define SPI_0_SCLK_DEV     PORT->Group[2]
#define SPI_0_SCLK_PIN     (18)

#define SPI_0_MISO_DEV     PORT->Group[2]
#define SPI_0_MISO_PIN     (19)

#define SPI_0_MOSI_DEV     PORT->Group[1]
#define SPI_0_MOSI_PIN     (30)

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM3->I2CM
#define I2C_0_IRQ           SERCOM3_IRQn
#define I2C_0_ISR           isr_sercom3
/* I2C 0 pin configuration */
#define I2C_0_PORT          (PORT->Group[0])
#define I2C_SDA             PIN_PA16
#define I2C_SCL             PIN_PA17
#define I2C_0_PINS          (PORT_PA16 | PORT_PA17)

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF       (0U)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC->MODE2
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             RTC->MODE0
#define RTT_IRQ             RTC_IRQn
#define RTT_IRQ_PRIO        10
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#define RTT_RUNSTDBY        (1)         /* Keep RTT running in sleep states */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
