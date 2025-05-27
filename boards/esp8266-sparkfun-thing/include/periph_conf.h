/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp8266_sparkfun-thing
 * @brief       Board specific configuration of MCU periphery for
 *              SparkFun ESP8266 Thing boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name   I2C configuration
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED      I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA        GPIO4           /**< SDA signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL        GPIO5           /**< SCL signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0),
 *        at maximum 8 channels.
 *
 * @note As long as PWM_DEV(0) is not initialized with \ref pwm_init, the GPIOs
 * declared as PWM channels can be used for other purposes.
 */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { GPIO0, GPIO4, GPIO5 }
#endif

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/**
 * @brief   SPI Flash chip size can not be determined by the chip and therefore
 *          must be explicitly set to 512 kbytes
 */
#define SPI_FLASH_CHIP_SIZE  0x80000

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
