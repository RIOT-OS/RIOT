/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_gpio_abc
 *
 * @{
 * @file
 * @brief       CPU/MCU specific definitions and constants for GPIO ABC
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CPU_GPIO_ABC_H
#define CPU_GPIO_ABC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Hardware specific constants to calculate precise timings
 * @{
 */
/**
 * @brief   CPU cycles per delay loop iteration
 */
#define GPIO_ABC_TICK_CYCLES        (6U)
/**
 * @brief   Overhead to compensate for in delay loop measured in CPU cycles
 *
 * This is the number of CPU cycles a call to @ref gpio_set_for and
 * @ref gpio_clear_for requires without no iterations of the delay loop
 */
#define GPIO_ABC_OVERHEAD_CYCLES    (24U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_GPIO_ABC_H */
/** @} */
