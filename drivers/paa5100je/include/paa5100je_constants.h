/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_paa5100je
 *
 * @{
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Register definitions for the PAA5100JE amd PMW3901 optical flow sensors
 * @{
 */
/** @brief Product ID (reset value: 0x49, read-only) */
#define REG_ID                   (0x00)

/** @brief Product revision ID (reset value: 0x00, read-only) */
#define REG_REV                  (0x01)

/** @brief Motion data ready flag (bitfield, indicates new data availability, read/write) */
#define REG_DATA_READY           (0x02)

/** @brief Motion burst register: block read of motion, delta X/Y, quality, shutter, read-only */
#define REG_MOTION_BURST         (0x16)

/** @brief Write 0x5A to reset internal registers/state machine (soft reset), write-only */
#define REG_POWER_UP_RESET       (0x3A)

/** @brief Sensor orientation/mirroring (read/write, flips axes depending on mounting) */
#define REG_ORIENTATION          (0x5B)

/** @brief Inverse Product ID (reset value: 0xB6, bitwise NOT of REG_ID, read-only) */
#define REG_INV_ID               (0x5F)

/** @brief Register for controlling the LED brightness (read/write) */
#define REG_LED_BRIGHTNESS       (0x6F)

/** @brief Magic value to turn the led off */
#define PAA5100JE_LED_OFF_VAL    (0x00)
/** @brief Magic value to set the led to medium brightness */
#define PAA5100JE_LED_MEDIUM_VAL (0x1C)
/** @brief Magic value to set the led to maximum brightness */
#define PAA5100JE_LED_MAX_VAL    (0xD5)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
