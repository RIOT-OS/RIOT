/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef OPENWSN_DEBUGPINS_H
#define OPENWSN_DEBUGPINS_H

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

#endif /* OPENWSN_DEBUGPINS_H */
