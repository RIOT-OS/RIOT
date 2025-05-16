/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_tsl2561
 * @brief       Internal addresses, registers, constants for the TSL2561 sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the TSL2561 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name TSL2561 identifier
 * @{
 */
#define TSL2561_ID                        (0x50)
/** @} */

/**
 * @name TSL2561 internals registers
 * @{
 */
#define TSL2561_REGISTER_CONTROL          (0x00)
#define TSL2561_REGISTER_TIMING           (0x01)
#define TSL2561_REGISTER_THRESHOLDLOW     (0x02)
#define TSL2561_REGISTER_THRESHOLDHIGH    (0x04)
#define TSL2561_REGISTER_INTERRUPT        (0x06)
#define TSL2561_REGISTER_ID               (0x0A)
#define TSL2561_REGISTER_CHAN0            (0x0C)
#define TSL2561_REGISTER_CHAN1            (0x0E)
/** @} */

/**
 * @name TSL2561 commands
 * @{
 */
#define TSL2561_COMMAND_MODE              (0x80)
#define TSL2561_COMMAND_CLEAR             (0x40)
#define TSL2561_COMMAND_WORD              (0x20)
#define TSL2561_COMMAND_BLOCK             (0x10)
/** @} */

/**
 * @name TSL2561 controls
 * @{
 */
#define TSL2561_CONTROL_POWERON           (0x03)
#define TSL2561_CONTROL_POWEROFF          (0x00)
/** @} */

/**
 * @name Internals constants
 * @{
 */
#define TSL2561_LUXSCALE                  (14)      /* use 2e14 scaling */
#define TSL2561_RATIOSCALE                (9)       /* use 2e9 scaling */
#define TSL2561_CHSCALE                   (10)      /* use 2e10 scaling on
                                                     * channel values by */
#define TSL2561_CHSCALE_TINT0             (0x7517)
#define TSL2561_CHSCALE_TINT1             (0x0FE7)

#define TSL2561_K1T                       (0x0040)
#define TSL2561_B1T                       (0x01f2)
#define TSL2561_M1T                       (0x01be)
#define TSL2561_K2T                       (0x0080)
#define TSL2561_B2T                       (0x0214)
#define TSL2561_M2T                       (0x02d1)
#define TSL2561_K3T                       (0x00c0)
#define TSL2561_B3T                       (0x023f)
#define TSL2561_M3T                       (0x037b)
#define TSL2561_K4T                       (0x0100)
#define TSL2561_B4T                       (0x0270)
#define TSL2561_M4T                       (0x03fe)
#define TSL2561_K5T                       (0x0138)
#define TSL2561_B5T                       (0x016f)
#define TSL2561_M5T                       (0x01fc)
#define TSL2561_K6T                       (0x019a)
#define TSL2561_B6T                       (0x00d2)
#define TSL2561_M6T                       (0x00fb)
#define TSL2561_K7T                       (0x029a)
#define TSL2561_B7T                       (0x0018)
#define TSL2561_M7T                       (0x0012)
#define TSL2561_K8T                       (0x029a)
#define TSL2561_B8T                       (0x0000)
#define TSL2561_M8T                       (0x0000)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
