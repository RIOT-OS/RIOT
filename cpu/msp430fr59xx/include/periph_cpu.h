/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_msp430fr59xx
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "msp430_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffff)

/**
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO_PIN(x, y)      ((gpio_t)(((x & 0xff) << 8) | (1 << (y & 0x07))))

/**
 * @brief   Define a custom type for GPIO interrupt context
 * @{
 */
#define HAVE_GPIO_ISR_CTX_T
typedef struct {
    void (*cb)(void *arg);      /**< interrupt callback */
    void *arg;                  /**< optional argument */
    bool both_edges;            /**< detect both rising and falling edges */
} gpio_isr_ctx_t;

/**
 * @brief   Available ports on MSP430 platforms
 */
enum {
    P1  = 1,                /**< PORT 1 */
    P2  = 2,                /**< PORT 2 */
    P3  = 3,                /**< PORT 3 */
    P4  = 4,                /**< PORT 4 */
    PJ  = 5,                /**< PORT J */
};

/**
 * @brief   Enable or disable a pin to be used by peripheral modules
 *
 * @param[in] pin     pin to (de-)select
 * @param[in] sel0    true for enabling peripheral selection 0 use, false for disabling it
 * @param[in] sel1    true for enabling peripheral selection 1 use, false for disabling it
 * @param[in] selComp true for enabling peripheral selection complement use, false for disabling it
 */
void gpio_periph_mode(gpio_t pin, bool sel0, bool sel1, bool selComp);

/**
 * @brief   Reset the edge for pins with GPIO_BOTH
 *
 * @param[in] pin     pin to reset
 */
void gpio_reset_edge(gpio_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
