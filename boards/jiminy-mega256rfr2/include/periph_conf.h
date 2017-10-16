/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2014 Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinoccio-mega256rfr2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Pinoccio Mega 256rfr2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */
#include "mutex.h"
#include "periph_cpu.h"

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_cpu.h"
#include "atmega_regs_common.h"
#include "periph_cpu_common.h"
#include <stdint.h>
#include <avr/io.h>
/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (8000000L)
/** @} */

/**
 * @brief xtimer configuration values
 * @{
 */
#define XTIMER_DEV					TIMER_DEV(0)   			// set TC0 as system counter
#define XTIMER_CHAN 				(0)				// choose channel 0
#define XTIMER_WIDTH                (16)			// 16bit timer
//#define XTIMER_SHIFT                (3)				// xtimer prescaler value, If the underlying hardware timer is running at a power of two multiple ofFor a 16 MHz hardware timer, set XTIMER_SHIFT to 4.
#define XTIMER_HZ                   (125000UL)	// set Timer frequency TODO think about slowing down timer for power saving
#define XTIMER_BACKOFF              (5000)			// TODO look into this , All timers that are less than XTIMER_BACKOFF microseconds in the future willjust spin. This is supposed to be defined per-device in e.g., periph_conf.h.
#define XTIMER_ISR_BACKOFF		(5000)		//When scheduling the next IRQ, if it is less than the backoff time in the future, just spin.
#define XTIMER_OVERHEAD				(1000)		//This value specifies the time a timer will be late if uncorrected, e.g., the system-specific xtimer execution time from timer ISR to executing a timer's callback's first instruction.
/** @} */


/**
 * @brief   Timer configuration
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
 * TODO Change the timer
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

#define TIMER_2				MEGA_TIMER1
#define TIMER_2_MASK        &TIMSK1
#define TIMER_2_FLAG        &TIFR1
#define TIMER_2_ISRA        TIMER1_COMPA_vect
#define TIMER_2_ISRB        TIMER1_COMPB_vect
#define TIMER_2_ISRC        TIMER1_COMPC_vect
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */

#define PWM_NUMOF			(1U)
/* To reduce confusion by steadily renaming hardware modules with different numbered drivers
 * namingconvention is not to start with 0 but to name driver with the number fitting the underlying hardware module*/
#define PWM_1				(0U)

static uint8_t scale[] = {1}; /** used to archieve better pwm frequency */
static uint8_t prescaler[] = {1}; /** used to start and stop timer by setting clock */

static const pwm_conf_t pwm_config[] = {
		{
				.dev = MEGA_TIMER1,
				.power_reg = &PRR0,
				.power_reg_bit = PRTIM1,
				.chan     = { { .pin = GPIO_PIN(PORT_B, 5), .cc_chan = 0 },
							{ .pin = GPIO_PIN(PORT_B, 6), .cc_chan = 1 },
							{ .pin = GPIO_PIN(PORT_B, 7), .cc_chan = 2 } },
				.scale_pointer = &(scale[0]),
				.prescaler_pointer = &(prescaler[0]),
				.bits = 16
		}
};

/**
 * @brief Analog comparator configuration
 */
#define AC_NUMOF  			(1U)
#define AC_0				(0U)

static const ac_conf_t ac_config[] = {
		{
				.acsr = &ACSR,
				.adcsrb = &ADCSRB
		}
};

/**
 * since current Analog comparator ISR is to slow, we'll hardcode it into the module
 * and use the define to not include the line if the capacity_module isn't used
 */
#define USES_CAPACITY_MODULE

/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (2U)

// UARTo is used for stdio
#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect
/** @} */


/**
 * @brief I2C configuartion
 */
#define I2C_NUMOF		(1U)
#define I2C_0			(0)

static uint8_t mask_value = 0x00;
static mutex_t my_mutex = MUTEX_INIT;
static const i2c_conf_t i2c_config[] = {
		{
				.scl_pin = GPIO_PIN(PORT_D, 0),
				.sda_pin = GPIO_PIN(PORT_D, 1),
				.mask = &mask_value,
				.used = &my_mutex
		}
};

/**
 * @brief RTC configuartion
 */
#define RTC_NUMOF	(1U)


/**
 * @brief SPI configuration
 *
 * The atmega256rfr has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 *
 * The fixed pins used, are: TODO which pins on pinoccio
 * MISO - PB3 (Pinoccio pin )
 * MOSI - PB2 (Pinoccio pin )
 * SCK  - PB1 (Pinoccio pin )
 * SS   - PB0 (Pinoccio pin )-> this pin is configured as output, but not used
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
#define SPI_0_EN            1           /* remove once SPI rework is done */
#define MEGA_PRR            PRR0        /* Power Reduction Register is PRR0 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
