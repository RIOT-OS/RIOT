/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_max31855
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the data read from the MAX31855 in bytes.
 */
#define MAX31855_DATA_SIZE (4)

/**
 * @brief Shift value for the thermocouple sign bit.
 */
#define MAX31855_THERMOCOUPLE_SIGN_SHIFT (31)

/**
   @brief Mask value for the thermocouple sign bit.
 */
#define MAX31855_THERMOCOUPLE_SIGN_MASK (1UL << MAX31855_THERMOCOUPLE_SIGN_SHIFT)

/**
 * @brief Shift value for the thermocouple integer bits.
 */
#define MAX31855_THERMOCOUPLE_INTEGER_SHIFT (20)

/**
 * @brief Mask value for the thermocouple integer bits.
 */
#define MAX31855_THERMOCOUPLE_INTEGER_MASK (0x7FFUL << MAX31855_THERMOCOUPLE_INTEGER_SHIFT)

/**
 * @brief Shift value for the thermocouple fractional half degree bit.
 */
#define MAX31855_THERMOCOUPLE_FRACTIONAL_HALF_SHIFT (19)

/**
 * @brief Mask value for the thermocouple fractional half degree bit.
 */
#define MAX31855_THERMOCOUPLE_FRACTIONAL_HALF_MASK (1UL << MAX31855_THERMOCOUPLE_FRACTIONAL_HALF_SHIFT)

/**
 * @brief Shift value for the thermocouple fractional quarter degree bit.
 */
#define MAX31855_THERMOCOUPLE_FRACTIONAL_QUARTER_SHIFT (18)

/**
 * @brief Mask value for the thermocouple fractional quarter degree bit.
 */
#define MAX31855_THERMOCOUPLE_FRACTIONAL_QUARTER_MASK (1UL << MAX31855_THERMOCOUPLE_FRACTIONAL_QUARTER_SHIFT)

/**
 * @brief Shift value for the internal sign bit.
 */
#define MAX31855_INTERNAL_SIGN_SHIFT (15)

/**
   @brief Mask value for the internal sign bit.
 */
#define MAX31855_INTERNAL_SIGN_MASK (1UL << MAX31855_INTERNAL_SIGN_SHIFT)

/**
 * @brief Shift value for the internal integer bits.
 */
#define MAX31855_INTERNAL_INTEGER_SHIFT (8)

/**
 * @brief Mask value for the internal integer bits.
 */
#define MAX31855_INTERNAL_INTEGER_MASK (0x7FUL << MAX31855_INTERNAL_INTEGER_SHIFT)

/**
 * @brief Shift value for the internal fractional half degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_HALF_SHIFT (7)

/**
 * @brief Mask value for the internal fractional half degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_HALF_MASK (1UL << MAX31855_INTERNAL_FRACTIONAL_HALF_SHIFT)

/**
 * @brief Shift value for the internal fractional quarter degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_QUARTER_SHIFT (6)

/**
 * @brief Mask value for the internal fractional quarter degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_QUARTER_MASK (1UL << MAX31855_INTERNAL_FRACTIONAL_QUARTER_SHIFT)

/**
 * @brief Shift value for the internal fractional eighth degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_EIGHTH_SHIFT (5)

/**
 * @brief Shift value for the internal fractional eighth degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_EIGHTH_MASK (1UL << MAX31855_INTERNAL_FRACTIONAL_EIGHTH_SHIFT)

/**
 * @brief Shift value for the internal fractional sixteenth degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_SIXTEENTH_SHIFT (4)

/**
 * @brief Shift value for the internal fractional sixteenth degree bit.
 */
#define MAX31855_INTERNAL_FRACTIONAL_SIXTEENTH_MASK (1UL << MAX31855_INTERNAL_FRACTIONAL_SIXTEENTH_SHIFT)

/**
 * @brief Shift value for the fault bit indicating a VCC short.
 */
#define MAX31855_FAULT_VCC_SHORT_SHIFT (2)

/**
 * @brief Mask value for the fault bit indicating a VCC short.
 */
#define MAX31855_FAULT_VCC_SHORT_MASK (1UL << MAX31855_FAULT_VCC_SHORT_SHIFT)

/**
 * @brief Shift value for the fault bit indicating a GND short.
 */
#define MAX31855_FAULT_GND_SHORT_SHIFT (1)

/**
 * @brief Mask value for the fault bit indicating a GND short.
 */
#define MAX31855_FAULT_GND_SHORT_MASK (1UL << MAX31855_FAULT_GND_SHORT_SHIFT)

/**
 * @brief Shift value for the fault bit indicating an open circuit.
 */
#define MAX31855_FAULT_OPEN_CIRCUIT_SHIFT (0)

/**
 * @brief Mask value for the fault bit indicating an open circuit.
 */
#define MAX31855_FAULT_OPEN_CIRCUIT_MASK (1UL << MAX31855_FAULT_OPEN_CIRCUIT_SHIFT)

/**
 * @brief Mask value for the fault bits.
 */
#define MAX31855_FAULT_MASK (MAX31855_FAULT_VCC_SHORT_MASK | \
                             MAX31855_FAULT_GND_SHORT_MASK | \
                             MAX31855_FAULT_OPEN_CIRCUIT_MASK)

#ifdef __cplusplus
}
#endif

/** @} */
