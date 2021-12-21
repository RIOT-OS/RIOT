/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Shared CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "cpu.h"
#include "macros/units.h"

#if defined(CPU_FAM_STM32F0)
#include "periph/f0/periph_cpu.h"
#elif defined(CPU_FAM_STM32F1)
#include "periph/f1/periph_cpu.h"
#elif defined(CPU_FAM_STM32F2)
#include "periph/f2/periph_cpu.h"
#elif defined(CPU_FAM_STM32F3)
#include "periph/f3/periph_cpu.h"
#elif defined(CPU_FAM_STM32F4)
#include "periph/f4/periph_cpu.h"
#elif defined(CPU_FAM_STM32F7)
#include "periph/f7/periph_cpu.h"
#elif defined(CPU_FAM_STM32G0)
#include "periph/g0/periph_cpu.h"
#elif defined(CPU_FAM_STM32G4)
#include "periph/g4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L0)
#include "periph/l0/periph_cpu.h"
#elif defined(CPU_FAM_STM32L1)
#include "periph/l1/periph_cpu.h"
#elif defined(CPU_FAM_STM32L4)
#include "periph/l4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L5)
#include "periph/l5/periph_cpu.h"
#elif defined(CPU_FAM_STM32U5)
#include "periph/u5/periph_cpu.h"
#elif defined(CPU_FAM_STM32WB)
#include "periph/wb/periph_cpu.h"
#elif defined(CPU_FAM_STM32WL)
#include "periph/wl/periph_cpu.h"
#endif

#include "periph/cpu_common.h"
#include "periph/cpu_dma.h"
#include "periph/cpu_eth.h"
#include "periph/cpu_gpio.h"
#include "periph/cpu_i2c.h"
#include "periph/cpu_ltdc.h"
#include "periph/cpu_pm.h"
#include "periph/cpu_pwm.h"
#include "periph/cpu_qdec.h"
#include "periph/cpu_spi.h"
#include "periph/cpu_timer.h"
#include "periph/cpu_uart.h"
#include "periph/cpu_usbdev.h"
#include "periph/cpu_wdt.h"

#ifdef MODULE_PERIPH_CAN
#include "candev_stm32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
#if !defined(CPU_FAM_STM32F0) && !defined(CPU_FAM_STM32L0) && \
    !defined(CPU_FAM_STM32L1) && !defined(CPU_FAM_STM32WL)
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
#endif
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @name    PTP clock configuration
 * @{
 */
#define HAVE_PTP_CLOCK_READ         1   /**< Native implementation available */
#define HAVE_PTP_CLOCK_SET          1   /**< Native implementation available */
#define HAVE_PTP_TIMER_SET_ABSOLUTE 1   /**< Native implementation available */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
