/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIOINT_H_
#define GPIOINT_H_

/**
 * @defgroup    drivers_gpioint GPIO IRQ Multiplexer
 * @ingroup     drivers
 * @brief       Provides an API to implement interrupt handlers on IO pins.
 *
 * Multiplexer and interrupt handling must be implemented platform specific.
 *
 * @{
 *
 * @file
 * @brief       GPIO IRQ Multiplexer interface
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * gpioint_flags:
 * Note: - We rely on the exact values for the edges.
 *       - These flags are extended in hal/drivers/device-gpio.h
 */
#define GPIOINT_DISABLE         0x00
#define GPIOINT_RISING_EDGE     0x01            ///< interrupt is generated on rising edge
#define GPIOINT_FALLING_EDGE    0x02            ///< interrupt is generated on falling edge
#define GPIOINT_DEBOUNCE        0x04            ///< debounce this interrupt

/**
 * @brief   GPIO IRQ callback function type
 * @param[in]       data        User defined callback data passed through gpioint_set
 * @param[in]       edge        A combination of GPIOINT_RISING_EDGE and GPIOINT_FALLING_EDGE
 */
typedef void(*fp_irqcb)(void);

/**
 * @brief   GPIO IRQ handler setup
 * @param[in]   port        CPU specific port number (starting at 0)
 * @param[in]   bitmask     One or more bits for which to set the handler
 * @param[in]   flags       A combination of #GPIOINT_RISING_EDGE and #GPIOINT_FALLING_EDGE
 * @param[in]   callback    A pointer to a handler function
 * @retval      true        successful
 * @retval      false       failed
 *
 * To enable interrupt handling for a pin flags and callback must be non-zero. To disable interrupt
 * handling flags and callback shall be zero.
 */
bool gpioint_set(int port, uint32_t bitmask, int flags, fp_irqcb callback);

void gpioint_init(void);

/** @} */
#endif /* GPIOINT_H_ */
