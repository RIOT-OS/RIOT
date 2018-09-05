/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp8266_olimex-mod
 * @brief       Board specific configuration of MCU periphery for
 *              Olimex MOD-WIFI-ESP8266-DEV boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name   I2C configuration
 * @{
 */
#ifndef I2C_NUMOF
#define I2C_NUMOF       (1)             /**< Number of I2C interfaces */
#endif
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

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
