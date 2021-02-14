/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy52
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
 * @name    Power switch pins definitions
 * @{
 */
#define SPK_PWR_CTRL_PIN    GPIO_PIN(0, 29) /**< Speaker power switch */
#define VDD_PWR_CTRL_PIN    GPIO_PIN(0, 30) /**< VDD power switch */
/** @} */

/**
 * @brief   Pin configuration for devices connected to the sx1509 I/O expander
 */
enum {
    SXIO_IOEXT0,            /**< digital GPIO, externally available */
    SXIO_IOEXT1,            /**< digital GPIO, externally available */
    SXIO_IOEXT2,            /**< digital GPIO, externally available */
    SXIO_IOEXT3,            /**< digital GPIO, externally available */
    SXIO_BAT_MON_EN,        /**< battery monitoring enable */
    SXIO_LIGHTWELL_G,       /**< green lightwell LED */
    SXIO_LIGHTWELL_B,       /**< blue lightwell LED */
    SXIO_LIGHTWELL_R,       /**< red lightwell LED */
    SXIO_MPU_PWR_CTRL,      /**< motion sensor power control */
    SXIO_MIC_PWR_CTRL,      /**< microphone power control */
    SXIO_CCS_PWR_CTRL,      /**< gas sensor power control */
    SXIO_CCS_RESET,         /**< gas sensor reset line */
    SXIO_CCS_WAKE,          /**< gas sensor wake line */
    SXIO_SENSE_LED_R,       /**< red sensor support LED */
    SXIO_SENSE_LED_G,       /**< green sensor support LED */
    SXIO_SENSE_LED_B,       /**< blue  sensor support LED */
};

/**
 * @name    LIS2DH12 low power accelerometer configuration
 * @{
 */
#define LIS2DH12_PARAM_I2C  I2C_DEV(1)
/** @} */

/**
 * @name    LPS22HB device address
 * @{
 */
#define LPSXXX_PARAM_ADDR   (0x5c)
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
