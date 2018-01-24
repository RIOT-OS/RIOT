/*
 * Copyright (C) Josua Arndt, Steffen Robertz 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "atmega_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Length of the CPU_ID in octets
 * @{
 */
#define CPUID_LEN           (10U)

/** @} */

/**
 * @name   Add 16 bit randome ending instead off 0000 to CPUID
 *
 *         To use this feature the Module AT86RFR2 is needed.
 * @{
 */
#define CPU_ID_ADD_RANDOME  (0)
/** @} */

/**
 * @name   Available ports on the ATmega256rfr family
 * @{
 */
enum {
    PORT_B  = 1,        /**< port B */
    PORT_D  = 3,        /**< port D */
    PORT_E  = 4,        /**< port E */
    PORT_F  = 5,        /**< port F */
    PORT_G  = 6,        /**< port G */
};
/** @} */
#define TIMER_CHAN          (3U) /**< Atmega256rfr2 timers have up to 3 capture-compare channels (TIMER 1/3) */

/**
 * @name   GPIO pin not defined
 * @{
 */
#ifndef GPIO_UNDEF
#define GPIO_UNDEF          (0xFFFF)
#endif
/** @}*/

/**
 * @name   PWM settings struct
 * @{
 */
typedef struct {
    unsigned int pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;
/** @}*/


/**
 * @name   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (2U)

/*UART0 is used for stdio*/
#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect
/** @} */


/**
 * @name   PWM configuration struct
 * @{
 */
typedef struct {
    mega_timer_t *dev;              /**< Timer used */
    pwm_chan_t chan[TIMER_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0} */
    REG8 *power_reg;                /**< Save Powerregister for poweron and off */
    uint8_t power_reg_bit;          /**< Save which bit needs to be set in powerregister */
    uint8_t *scale_pointer;         /**< Scale variable used to map small resolution values to max 16bit */
    uint8_t *prescaler_pointer;     /**< remember calculated prescaler, as it changes during poweroff and on */
    uint8_t bits;                   /**< Timer Size, e.g. 8bit or 16bit */
} pwm_conf_t;
/** @} */

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD1) | (1 << PORTD0)
/** @} */

/**
 * @brief    analog comparator setting struct
 * @{
 */
typedef struct {
    unsigned int in1;       /**< Comparator Input Pin 1 */
    unsigned int in2;       /**< Comparator Input Pin 2 */
} ac_conf_t;
/** @} */

/**
 * @name SPI configuration
 *
 * The atmega256rfr has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
#define SPI_0_EN            1           /* remove once SPI rework is done */
#define MEGA_PRR            PRR0        /* Power Reduction Register is PRR0 */
/** @} */

/**
 * @name ADC Configuration
 * @{
 */
#define ADC_NUMOF           (8U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
