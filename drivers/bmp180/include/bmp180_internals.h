/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_bmp180
 * @brief       Internal addresses, registers, constants for the BMP180 sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the BMP180 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    BMP180 I2C address
 * @{
 */
#define BMP180_ADDR                   (0x77) /* 7 bit address */
/** @} */

/**
 * @name    BMP180 registers
 * @{
 */
#define BMP180_REGISTER_ID            (0xD0)
#define BMP180_REGISTER_CONTROL       (0xF4)
#define BMP180_REGISTER_DATA          (0xF6)
#define BMP180_TEMPERATURE_COMMAND    (0x2E)
#define BMP180_PRESSURE_COMMAND       (0x34)
#define BMP180_CALIBRATION_AC1        (0xAA)
/** @} */

/**
 * @name    Oversampling modes delays (micros)
 * @{
 */
#define BMP180_ULTRALOWPOWER_DELAY_MS (5UL)     /**< Ultra low power delay (ms) */
#define BMP180_STANDARD_DELAY_MS      (8UL)     /**< Standard resolution delay (ms) */
#define BMP180_HIGHRES_DELAY_MS       (14UL)    /**< High resolution delay (ms) */
#define BMP180_ULTRAHIGHRES_DELAY_MS  (26UL)    /**< Ultra high resolution delay (ms) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
