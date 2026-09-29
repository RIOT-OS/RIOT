/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Provides an adaption of OpenWSN debug pin handling
 *              to RIOTs handling of GPIOs.
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   struct holding configuration of OpenWSN debug pins
 */
typedef struct debugpins_config {
    gpio_t frame;       /**< debug pin for frames */
    gpio_t slot;        /**< debug pin for slots  */
    gpio_t fsm;         /**< debug pin for fsm */
    gpio_t task;        /**< debug pin for tasks */
    gpio_t isr;         /**< debug pin for interrupt service routines */
    gpio_t radio;       /**< debug pin for the radio */
} debugpins_config_t;

/**
 * Sets the debug pins for a specific board for OpenWSN
 *
 * @param[in] user_config   A configuration of GPIO pins used for debugging.
 *                          Unused pins need to be defined as GPIO_UNDEF.
 */
void openwsn_debugpins_init(const debugpins_config_t *user_config);

#ifdef __cplusplus
}
#endif
