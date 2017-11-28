/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-mega256rfr2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Jiminy Mega 256rfr2 board
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <avr/io.h>
#include "periph_cpu.h"
#include "atmega_regs_common.h"
#include "periph_cpu_common.h"

/**
 * @name   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (8000000L)
/** @} */

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (125000UL)
#define XTIMER_BACKOFF      (5000)
#define XTIMER_ISR_BACKOFF  (5000)
#define XTIMER_OVERHEAD     (1000)
/** @} */


/**
 * @name   Timer configuration
 *
 * ATTETION Timer 0 is used for Xtimer which is system Timer
 *
 * The timer driver only supports the four 16-bit timers
 * (Timer1, TST, Timer3, Timer4, Timer5), so those are the Timer we can use here.
 * Timer 1, TST, PORT B5/6/7 Out, Port D4/6 In,  Analog Comparator Input Capture, Output Compare Modulator, PWM
 * Timer 3, TST, PORT E3/4/5 Out, Port E/6/7 In, Input or Output Compare and PWM Output
 * Timer 4, TST, It can not be connected to any I/O Pin,
 * Timer 5, TST, It can not be connected to any I/O Pin,
 *
 * Using Timer 4 and 5 seems to be the best choice
 * @{
 */
#define TIMER_NUMOF         (3U)

#define TIMER_0             MEGA_TIMER4
#define TIMER_0_MASK        &TIMSK4
#define TIMER_0_FLAG        &TIFR4
#define TIMER_0_ISRA        TIMER4_COMPA_vect
#define TIMER_0_ISRB        TIMER4_COMPB_vect
#define TIMER_0_ISRC        TIMER4_COMPC_vect

#define TIMER_1             MEGA_TIMER5
#define TIMER_1_MASK        &TIMSK5
#define TIMER_1_FLAG        &TIFR5
#define TIMER_1_ISRA        TIMER5_COMPA_vect
#define TIMER_1_ISRB        TIMER5_COMPB_vect
#define TIMER_1_ISRC        TIMER5_COMPC_vect

#define TIMER_2             MEGA_TIMER1
#define TIMER_2_MASK        &TIMSK1
#define TIMER_2_FLAG        &TIFR1
#define TIMER_2_ISRA        TIMER1_COMPA_vect
#define TIMER_2_ISRB        TIMER1_COMPB_vect
#define TIMER_2_ISRC        TIMER1_COMPC_vect
/** @} */

/**
 * @name   PWM configuration
 * @{
 */

/* To reduce confusion by steadily renaming hardware modules with different numbered drivers
 * namingconvention is not to start with 0 but to name driver with the number fitting the underlying timer module*/
#define PWM_1       (0U)

static uint8_t scale[] = { 1 };     /** used to archieve better pwm frequency */
static uint8_t prescaler[] = { 1 }; /** used to start and stop timer by setting clock */

static const pwm_conf_t pwm_config[] = {
    {
        .dev = MEGA_TIMER1,
        .power_reg = &PRR0,
        .power_reg_bit = PRTIM1,
        .chan = { { .pin = GPIO_PIN(PORT_B, 5), .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_B, 6), .cc_chan = 1 },
                  { .pin = GPIO_PIN(PORT_B, 7), .cc_chan = 2 } },
        .scale_pointer = &(scale[0]),
        .prescaler_pointer = &(prescaler[0]),
        .bits = 16
    }
};
#define PWM_NUMOF   (sizeof(pwm_config)/sizeof(pwm_conf_t))
/** @} */

/**
 * @name Analog comparator configuration
 * @{
 */
#define AC_0        (0U)

static const ac_conf_t ac_config[] = {
    {
        .acsr = &ACSR,
        .adcsrb = &ADCSRB
    }
};

#define AC_NUMOF    (sizeof(ac_config)/sizeof(ac_conf_t))
/*
 * since current Analog comparator ISR is to slow, we'll hardcode it into the module
 * and use the define to not include the line if the capacity_module isn't used
 */
#define USES_CAPACITY_MODULE
/** @} */

/**
 * @name RTC configuartion
 * @{
 */
#define RTC_NUMOF   (1U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
