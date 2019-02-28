/*
 * Copyright (C) 2018 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_il3829
 * @{
 *
 * @file
 * @brief       Device driver implementation for the il3829 display controller
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#ifndef IL3829_INTERNAL_H
#define IL3829_INTERNAL_H

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IL3829 SPI commands
 * @{
 */
#define IL3829_CMD_DRIVER_OUTPUT_CONTROL (0x01)
#define IL3829_CMD_GATE_DRIVING_VOLTAGE_CONTROL (0x03) /* unused */
#define IL3829_CMD_SOURCE_DRIVING_VOLTAGE_CONTROL (0x04) /* unused */
#define IL3829_CMD_DISPLAY_CONTROL (0x07) /* unused */
#define IL3829_CMD_GATE_AND_SOURCE_NON_OVERLAP_PERIOD_CONTROL (0x0B) /* unused */
#define IL3829_CMD_BOOSTER_SOFT_START_CONTROL (0x0C)
#define IL3829_CMD_GATE_SCAN_START_POSITION (0x0F) /* unused */
#define IL3829_CMD_DEEP_SLEEP_MODE (0x10)
#define IL3829_CMD_DATA_ENTRY_MODE_SETTING (0x11)
#define IL3829_CMD_SWRESET (0x12)
#define IL3829_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE (0x1A) /* unused */
#define IL3829_CMD_TEMPERATURE_SENSOR_CONTROL_READ (0x1B) /* unused */
#define IL3829_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE_CMD (0x1C) /* unused */
#define IL3829_CMD_TEMPERATURE_SENSOR_CONTROL_LOAD (0x1D) /* unused */
#define IL3829_CMD_MASTER_ACTIVATION (0x20)
#define IL3829_CMD_DISPLAY_UPDATE_CONTROL_1 (0x21) /* unused */
#define IL3829_CMD_DISPLAY_UPDATE_CONTROL_2 (0x22)
#define IL3829_CMD_WRITE_RAM (0x24)
#define IL3829_CMD_READ_RAM (0x25) /* unused */
#define IL3829_CMD_VCOM_SENSE (0x28) /* unused */
#define IL3829_CMD_VCOM_SENSE_DURATION (0x29) /* unused */
#define IL3829_CMD_PROGRAM_VCOM_OTP (0x2A) /* unused */
#define IL3829_CMD_WRITE_VCOM_REGISTER (0x2C)
#define IL3829_CMD_READ_OTP_REGISTERS (0x2D) /* unused */
#define IL3829_CMD_PROGRAM_WS_OTP (0x30) /* unused */
#define IL3829_CMD_WRITE_LUT_REGISTER (0x32)
#define IL3829_CMD_READ_LUT_REGISTER (0x33) /* unused */
#define IL3829_CMD_PROGRAM_OTP_SELECTION (0x36) /* unused */
#define IL3829_CMD_OTP_SELECTION_CONTROL (0x37) /* unused */
#define IL3829_CMD_SET_DUMMY_LINE_PERIOD (0x3A)
#define IL3829_CMD_SET_GATE_LINE_WIDTH (0x3B)
#define IL3829_CMD_BORDER_WAVEFORM_CONTROL (0x3C) /* unused */
#define IL3829_CMD_SET_RAM_X (0x44)
#define IL3829_CMD_SET_RAM_Y (0x45)
#define IL3829_CMD_SET_RAM_X_ADDR_COUNTER (0x4E)
#define IL3829_CMD_SET_RAM_Y_ADDR_COUNTER (0x4F)
#define IL3829_CMD_NOP (0xFF)
/** @} */

/**
 * @name    IL3829 Waiting estimates in microseconds
 * @{
 */
#define IL3829_WAIT_UPDATE_FULL (1200 * US_PER_MS)
#define IL3829_WAIT_UPDATE_PART (300 * US_PER_MS)
#define IL3829_WAIT_ACTIVATION  (80 * US_PER_MS)
#define IL3829_WAIT_RESET       (1 * US_PER_MS)
/** @} */

/**
 * @name    IL3829 lookup table for a full display refresh
 */
static const uint8_t il3829_lut_default_full[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * @name    IL3829 lookup table for a partial display refresh
 */
static const uint8_t il3829_lut_default_part[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00,
    0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * @name    Common display initialisation code
 */
static void il3829_display_init(il3829_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* IL3829_INTERNAL_H */
