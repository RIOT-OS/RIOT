/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega2560
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "periph_cpu_common.h"
#include "atmega_regs_common.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Available ports on the ATmega256rfr family
 */
enum {
    // PORT_A = 0,       /**< port A */ /* not connected to pins for megaRF*/
    PORT_B = 1,       /**< port B */
    // PORT_C = 2,       /**< port C */ /* not connected to pins for megaRF*/
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
};

/**
 * @brief   Atmega256rfr2 timers have 3 capture-compare channels (TIMER 1/3) -> best suitable for
 * 			PWM and especially RGB Leds
 */
#define TIMER_CHAN          (3U)

typedef unsigned int gpio_t;
#ifndef GPIO_UNDEF
/**
 * @brief   GPIO pin not defined
 */
#define GPIO_UNDEF          (0xFFFF)
#endif
/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;


/**
 * @brief   PWM configuration
 */

typedef struct {
	mega_timer_t *dev;               /**< Timer used */
    pwm_chan_t chan[TIMER_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0} */
    REG8 *power_reg;			/**< Save Powerregister for poweron and off */
    uint8_t power_reg_bit;		/**< Save which bit needs to be set in powerregister */
    uint8_t *scale_pointer;
    uint8_t *prescaler_pointer;
    uint8_t bits;
} pwm_conf_t;

/**
 * Struct definition for I2C
 */
typedef struct{
	gpio_t scl_pin;
	gpio_t sda_pin;
	uint8_t *mask;
	mutex_t *used;
} i2c_conf_t;

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD1) | (1 << PORTD0)
/** @} */

/**
 * @brief setup analog comparator setting struct
 */
typedef struct {
	REG8 *acsr;
	REG8 *adcsrb;
	gpio_t in1;
	gpio_t in2;
}ac_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
