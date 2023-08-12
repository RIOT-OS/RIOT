/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       Shared CPU specific definitions for the GD32E23x family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

// #include <limits.h>
#include <inttypes.h>

#include "cpu.h"
// #include "macros/units.h"
// #include "gd32e23x.h"
// #include "gd32e23x_gpio.h"

// #if defined(CPU_FAM_GD32E23X)
// #include "vendor/GD32E23x_Firmware_Library/GD32E23x_standard_peripheral/Include/gd32e23x_gpio.h"
// #include "vendor/GD32E23x_Firmware_Library/GD32E23x_standard_peripheral/Include/gd32e23x_usart.h"
// #endif

// #ifdef MODULE_PERIPH_CAN
// #include "candev_stm32.h"
// #endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

// /**
//  * @brief   ADC channel configuration data
//  */
// typedef struct {
//     gpio_t pin;             /**< pin connected to the channel */
//     uint8_t chan;           /**< CPU ADC channel connected to the pin */
// } adc_conf_t;

// /**
//  * @brief   DAC line configuration data
//  */
// typedef struct {
//     gpio_t pin;             /**< pin connected to the line */
//     uint8_t chan;           /**< DAC device used for this line */
// } dac_conf_t;

// /**
//  * @name    PTP clock configuration
//  * @{
//  */
// #define HAVE_PTP_CLOCK_READ         1   /**< Native implementation available */
// #define HAVE_PTP_CLOCK_SET          1   /**< Native implementation available */
// #define HAVE_PTP_TIMER_SET_ABSOLUTE 1   /**< Native implementation available */
// /** @} */

/**
 * @brief   Available peripheral buses
 */
typedef enum {
  //check order here, VF103 has AHB, APB1, APB2
  //no definition of AHB 1 vs 2 (AHB2 is only used for GPIO and isn't adjustable in any way?)
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
    AHB,            /**< AHB bus */
} bus_t;

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A  = 0,            /**< port A */
#endif
#ifdef GPIOB
    PORT_B  = 1,            /**< port B */
#endif
#ifdef GPIOC
    PORT_C  = 2,            /**< port C */
#endif
#ifdef GPIOD
    PORT_D  = 3,            /**< port D */
#endif
#ifdef GPIOE
    PORT_E  = 4,            /**< port E */
#endif
#ifdef GPIOF
    PORT_F  = 5,            /**< port F */
#endif
};

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Override alternative GPIO mode options
 */
//TODO need to remap these.  the values from GD32VF103 AF PP/OD are not the same as for E23 AF OD/PP and they are in different registers
typedef enum {
    GPIO_AF_OUT_PP  = 0xb,  /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD  = 0xf,  /**< alternate function output - open-drain */
} gpio_af_t;

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
/* Ensure the internal "count" variable stays within the uint32 bounds.
   This variable corresponds to max_time * RTC_FREQ / MS_PER_SEC. On fe310,
   RTC_FREQ is 32768Hz. The 15 right shift is equivalent to a division by RTC_FREQ.
 */
#define NWDT_TIME_UPPER_LIMIT           ((UINT32_MAX >> 15)  * MS_PER_SEC + 1)
/** @} */

/**
 * @brief   UART interrupt priority
 */
#define UART_ISR_PRIO       (2)

void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
