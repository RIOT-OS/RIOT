/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Board specific definitions for the QN9080DK base board
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "mtd.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED configuration
 * @{
 */
#define LED_RED_PIN      GPIO_PIN(PORT_A, 31)
#define LED_GREEN_PIN    GPIO_PIN(PORT_A, 25)
#define LED_BLUE_PIN     GPIO_PIN(PORT_A, 13)
/** @} */

/**
 * @name    User buttons
 * @{
 */
#define BTN1_PIN         GPIO_PIN(PORT_A, 24)
#define BTN1_MODE        GPIO_IN_PU
#define BTN2_PIN         GPIO_PIN(PORT_A, 19)
#define BTN2_MODE        GPIO_IN_PU
/** @} */

/**
 * @name    MMA8652FC 3-axis accelerometer bus configuration
 *
 * The MMA8652FC in the QN9080DK is only available when JP11 is shorted (the
 * factory default).
 * @{
 */
#define MMA8X5X_PARAM_I2C       I2C_DEV(0)
#define MMA8X5X_PARAM_ADDR      0x1D
#define MMA8X5X_PARAM_TYPE      (MMA8X5X_TYPE_MMA8652)
/** @} */

/**
 * @name MTD configuration
 * @{
 */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
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
