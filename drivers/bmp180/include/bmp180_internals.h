/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bmp180 BMP180
 * @ingroup     drivers_sensors
 * @brief       Internal addresses, registers, constants for the BMP180 sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the BMP180 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BMP180_REGS_H
#define BMP180_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name BMP180 I2C address
 * @{
 */
#define BMP180_ADDR                   (0x77) /* 7 bit address */
/** @} */

/**
 * @name BMP180 registers
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
 * @name Oversampling modes delays (micros)
 * @{
 */
#define BMP180_ULTRALOWPOWER_DELAY    (5000UL)
#define BMP180_STANDARD_DELAY         (8000UL)
#define BMP180_HIGHRES_DELAY          (14000UL)
#define BMP180_ULTRAHIGHRES_DELAY     (26000UL)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BMP180_REGS_H */
/** @} */
