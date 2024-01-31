/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
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
 * @brief           GPIO LL CPU definitions for the STM32 family
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CPU_GPIO_LL_H
#define PERIPH_CPU_GPIO_LL_H

#include <stdalign.h>
#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hide this from Doxygen to avoid merging implementation details into
 * public view on type */
#ifndef DOXYGEN

#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 0,
    GPIO_DRIVE_STRONGEST = 0
} gpio_drive_strength_t;

/* Modern STM32 GPIO config registers with the OSPEEDR register support full
 * 4 slew rates, legacy STM32F1 style only have three slew rates. We define
 * slow and fast to the same value, so that we have three options:
 * 1. SLOWEST: 2 MHZ
 * 2. SLOW: 10 MHZ
 * 3. FAST/FASTEST: 50 MHz
 */
#if defined(GPIO_OSPEEDR_OSPEED0) || defined(GPIO_OSPEEDER_OSPEEDR0) \
    || defined(GPIO_OSPEEDER_OSPEED0) || defined(GPIO_OSPEEDR_OSPEEDR0)
#  define STM32_HAS_OSPEED  1
#else
#  define STM32_HAS_OSPEED  0
#endif

#define HAVE_GPIO_SLEW_T
#if STM32_HAS_OSPEED
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 1,
    GPIO_SLEW_FAST = 2,
    GPIO_SLEW_FASTEST = 3,
} gpio_slew_t;
#else
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 0,
    GPIO_SLEW_FAST = 1,
    GPIO_SLEW_FASTEST = 2,
} gpio_slew_t;
#endif

#define HAVE_GPIO_IRQ_TRIG_T
/*
 * Layout:
 *      7 6 5 4 3 2 1 0
 *      +-+-+-+-+-+-+-+-+
 *      |   RFU   |T|L|H|
 *      +-+-+-+-+-+-+-+-+
 *
 * RFU = Reserved for future use
 * T   = Trigger mode (1 = Level triggered, 0 = Edge triggered)
 * L   = Low (1 = low level / falling edge)
 * H   = High (1 = high level / rising edge)
 *
 * Note: This layout overlaps with gpio_flank_t by intent
 */
typedef enum {
    GPIO_TRIGGER_EDGE_RISING    = 0x1,
    GPIO_TRIGGER_EDGE_FALLING   = 0x2,
    GPIO_TRIGGER_EDGE_BOTH      = GPIO_TRIGGER_EDGE_RISING | GPIO_TRIGGER_EDGE_FALLING,
    GPIO_TRIGGER_LEVEL          = 0x4,
    GPIO_TRIGGER_LEVEL_HIGH     = GPIO_TRIGGER_LEVEL | GPIO_TRIGGER_EDGE_RISING,
    GPIO_TRIGGER_LEVEL_LOW      = GPIO_TRIGGER_LEVEL | GPIO_TRIGGER_EDGE_FALLING,
} gpio_irq_trig_t;

#define HAVE_GPIO_STATE_T
typedef enum {
    GPIO_OUTPUT_PUSH_PULL,
    GPIO_OUTPUT_OPEN_DRAIN,
    GPIO_OUTPUT_OPEN_SOURCE,
    GPIO_INPUT,
    GPIO_USED_BY_PERIPHERAL,
    GPIO_DISCONNECT,
} gpio_state_t;

#define HAVE_GPIO_PULL_T
typedef enum {
    GPIO_FLOATING,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
    GPIO_PULL_KEEP,
} gpio_pull_t;

#define HAVE_GPIO_CONF_T
typedef union gpio_conf_stm32 gpio_conf_t;

#endif /* ndef Doxygen */

/**
 * @brief       GPIO pin configuration for STM32 MCUs.
 * @ingroup     drivers_periph_gpio_ll
 */
union gpio_conf_stm32 {
    uint16_t bits;  /**< the raw bits */
    struct {
        /**
         * @brief   State of the pin
         */
        gpio_state_t state              : 3;
        /**
         * @brief   Pull resistor configuration
         */
        gpio_pull_t pull                : 2;
        /**
         * @brief   Configure the slew rate of outputs
         *
         * @warning If the requested slew rate is not available, the closest fit
         *          supported will be configured instead.
         *
         * This value is ignored *unless* @ref gpio_conf_stm32::state is
         * configured to @ref GPIO_OUTPUT_PUSH_PULL or @ref GPIO_OUTPUT_OPEN_DRAIN.
         */
        gpio_slew_t slew_rate           : 2;
        /**
         * @brief   Whether to disable the input Schmitt trigger
         *
         * @details This could be called `schmitt_trigger` with inverse
         *          meaning, but the API contract says that additional
         *          members in the structure should have a sane
         *          default when zero.
         *
         * This value is ignored *unless* @ref gpio_conf_stm32::state is
         * configured to @ref GPIO_INPUT.
         */
        bool schmitt_trigger_disabled   : 1;
        /**
         * @brief   Initial value of the output
         *
         * Ignored if @ref gpio_conf_stm32::state is set to @ref GPIO_INPUT or
         * @ref GPIO_DISCONNECT. If the pin was previously in a high impedance
         * state, it is guaranteed to directly transition to the given initial
         * value.
         *
         * @ref gpio_ll_query_conf will write the current value of the specified
         * pin here, which is read from the input register when the state is
         * @ref GPIO_INPUT, otherwise the state from the output register is
         * consulted.
         */
        bool initial_value          : 1;
        uint8_t                     : 7;    /*< padding */
    };
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_GPIO_LL_H */
/** @} */
