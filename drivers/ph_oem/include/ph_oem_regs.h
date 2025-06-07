/*
 * SPDX-FileCopyrightText: 2019 University of Applied Sciences Emden / Leer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ph_oem
 * @{
 *
 * @file
 * @brief       Register definitions for the Atlas Scientific pH OEM sensor.
 *
 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Atlas Scientific pH OEM register addresses
 *
 * All registers in the pH OEM are 8 bit wide and transmitted MSB first.
 *
 */
typedef enum ph_oem_reg {
    PH_OEM_REG_DEVICE_TYPE              = 0x00, /**< Device type register (read only) */
    PH_OEM_REG_FIRMWARE_VERSION         = 0x01, /**< Firmware version register (read only) */
    PH_OEM_REG_UNLOCK                   = 0x02, /**< SMBus/I²C address lock/unlock register (read/write) */
    PH_OEM_REG_ADDRESS                  = 0x03, /**< SMBus/I²C address register (read/write) */
    PH_OEM_REG_INTERRUPT                = 0x04, /**< Interrupt control register (read/write) */
    PH_OEM_REG_LED                      = 0x05, /**< LED control register (read/write) */
    PH_OEM_REG_HIBERNATE                = 0x06, /**< Active/hibernate register (read/write) */
    PH_OEM_REG_NEW_READING              = 0x07, /**< New reading available register (read/write)  */
    PH_OEM_REG_CALIBRATION_BASE         = 0x08, /**< Calibration value register base address. Register order is: MSB, high byte, low byte, LSB (0x08-0x0B) (read/write) */
    PH_OEM_REG_CALIBRATION_REQUEST      = 0x0C, /**< Calibration request register (read/write) */
    PH_OEM_REG_CALIBRATION_CONFIRM      = 0x0D, /**< Calibration confirm register (read/write) */
    PH_OEM_REG_TEMP_COMPENSATION_BASE   = 0x0E, /**< Temperature compensation register base address. Register order is: MSB, high byte, low byte, LSB (0x0E-0x11) (read/write) */
    PH_OEM_REG_TEMP_CONFIRMATION_BASE   = 0x12, /**< Temperature confirm register base address. Register order is: MSB, high byte, low byte, LSB (0x12-0x15) (read only) */
    PH_OEM_REG_PH_READING_BASE          = 0x16, /**< pH reading register base address, order= MSB, high byte, low byte, LSB (0x16-0x19) (read only) */

} ph_oem_reg_t;

/**
 * @brief Device ID of the @ref PH_OEM_REG_DEVICE_TYPE register of a pH OEM sensor
 */
#define PH_OEM_DEVICE_TYPE_ID  0x01

#ifdef __cplusplus
}
#endif

/** @} */
