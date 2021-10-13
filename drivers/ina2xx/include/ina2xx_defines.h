/*
 * Copyright (C) 2015 Eistec AB
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ina2xx
 * @{
 *
 * @file
 * @brief       Various definitions for Texas Instruments INA219/INA220
 *              Bi-Directional CURRENT/POWER MONITOR with Two-Wire Interface
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef INA2XX_DEFINES_H
#define INA2XX_DEFINES_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief INA2XX register addresses
 *
 * All registers in the INA2XX are 16 bit wide and transmitted MSB first.
 */
typedef enum ina2xx_reg {
    INA2XX_REG_CONFIGURATION = 0x00, /**< Configuration register (read/write) */
    INA2XX_REG_SHUNT_VOLTAGE = 0x01, /**< Shunt voltage register (read only) */
    INA2XX_REG_BUS_VOLTAGE   = 0x02, /**< Bus voltage register (read only) */
    INA2XX_REG_POWER         = 0x03, /**< Power register (read only) */
    INA2XX_REG_CURRENT       = 0x04, /**< Current register (read only) */
    INA2XX_REG_CALIBRATION   = 0x05, /**< Calibration register (read/write) */
} ina2xx_reg_t;

/**
 * @name Flags in the INA2XX Bus Voltage Register
 * @{
 */
#define INA2XX_VBUS_CNVR    (0x2)   /**< Unread value in power register ready */
#define INA2XX_VBUS_OVF     (0x1)   /**< Math overflow during conversion */
/** @} */

/**
 * @name Special configuration register values
 * @{
 */
#define INA2XX_RESET        (0x8000)/**< Write to config reg to reset device */
#define INA2XX_DEFCONFIG    (0x399f)/**< Default config after reset */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* INA2XX_DEFINES_H */
/** @} */
