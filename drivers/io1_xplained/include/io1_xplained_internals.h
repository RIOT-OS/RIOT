/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_io1_xplained IO1_XPLAINED
 * @ingroup     drivers_sensors
 * @brief       Internal addresses, constants for the IO1 Xplained extension.
 * @{
 *
 * @file
 * @brief       Internal addresses, constants for the IO1 Xplained extension.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef IO1_XPLAINED_INTERNALS_H
#define IO1_XPLAINED_INTERNALS_H

#include "cpu.h"
#include "periph_cpu.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name IO1 Xplained I2C addresses
 * @{
 */
#define TEMPERATURE_BASE_ADDR          (0x48)
#define TEMPERATURE_DEFAULT_ADDR       (0x07)
/** @} */

/**
 * @name IO1 Xplained LED pin
 * @{
 */
#define IO1_LED_PIN                    GPIO_PIN(0,18)
/** @} */

/**
 * @name IO1 Xplained gpio pins
 * @{
 */
#define IO1_GPIO1_PIN                  GPIO_PIN(0,13)
#define IO1_GPIO2_PIN                  GPIO_PIN(0,28)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_INTERNALS_H */
/** @} */
