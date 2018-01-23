/*
 * Copyright (C) 2017 Feie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_thingy52 Nordic Thingy:52
 * @ingroup     boards
 * @brief       Support for the Nordic Thingy:52 board
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Nordic Thingy:52 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 11)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    LIS2DH12 low power accelerometer configuration
 * @{
 */
#define LIS2DH12_PARAM_I2C  I2C_DEV(1)
/** @} */


/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
