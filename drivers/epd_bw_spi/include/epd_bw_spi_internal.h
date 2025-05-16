/*
 * Copyright (C) 2019 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_epd_bw_spi
 * @{
 *
 * @file
 * @brief       Device driver implementation for the epd_bw_spi display controller
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    EPD_BW_SPI SPI commands
 * @brief   Commands used for controlling EPD displays.
 *          These are surprisingly portable.
 * @{
 */
#define EPD_BW_SPI_CMD_DRIVER_OUTPUT_CONTROL                      (0x01)
#define EPD_BW_SPI_CMD_GATE_DRIVING_VOLTAGE_CONTROL               (0x03)    /* unused */
#define EPD_BW_SPI_CMD_SOURCE_DRIVING_VOLTAGE_CONTROL             (0x04)    /* unused */
#define EPD_BW_SPI_CMD_DISPLAY_CONTROL                            (0x07)    /* unused */
#define EPD_BW_SPI_CMD_GATE_AND_SOURCE_NON_OVERLAP_PERIOD_CONTROL (0x0B)    /* unused */
#define EPD_BW_SPI_CMD_BOOSTER_SOFT_START_CONTROL                 (0x0C)
#define EPD_BW_SPI_CMD_GATE_SCAN_START_POSITION                   (0x0F)    /* unused */
#define EPD_BW_SPI_CMD_DEEP_SLEEP_MODE                            (0x10)
#define EPD_BW_SPI_CMD_DATA_ENTRY_MODE_SETTING                    (0x11)
#define EPD_BW_SPI_CMD_SWRESET                                    (0x12)
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE           (0x1A)    /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_READ            (0x1B)    /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE_CMD       (0x1C)    /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_LOAD            (0x1D)    /* unused */
#define EPD_BW_SPI_CMD_MASTER_ACTIVATION                          (0x20)
#define EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_1                   (0x21)    /* unused */
#define EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_2                   (0x22)
#define EPD_BW_SPI_CMD_WRITE_RAM                                  (0x24)
#define EPD_BW_SPI_CMD_READ_RAM                                   (0x25)    /* unused */
#define EPD_BW_SPI_CMD_VCOM_SENSE                                 (0x28)    /* unused */
#define EPD_BW_SPI_CMD_VCOM_SENSE_DURATION                        (0x29)    /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_VCOM_OTP                           (0x2A)    /* unused */
#define EPD_BW_SPI_CMD_WRITE_VCOM_REGISTER                        (0x2C)
#define EPD_BW_SPI_CMD_READ_OTP_REGISTERS                         (0x2D)    /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_WS_OTP                             (0x30)    /* unused */
#define EPD_BW_SPI_CMD_WRITE_LUT_REGISTER                         (0x32)
#define EPD_BW_SPI_CMD_READ_LUT_REGISTER                          (0x33)    /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_OTP_SELECTION                      (0x36)    /* unused */
#define EPD_BW_SPI_CMD_OTP_SELECTION_CONTROL                      (0x37)    /* unused */
#define EPD_BW_SPI_CMD_SET_DUMMY_LINE_PERIOD                      (0x3A)
#define EPD_BW_SPI_CMD_SET_GATE_LINE_WIDTH                        (0x3B)
#define EPD_BW_SPI_CMD_BORDER_WAVEFORM_CONTROL                    (0x3C)    /* unused */
#define EPD_BW_SPI_CMD_SET_RAM_X                                  (0x44)
#define EPD_BW_SPI_CMD_SET_RAM_Y                                  (0x45)
#define EPD_BW_SPI_CMD_SET_RAM_X_ADDR_COUNTER                     (0x4E)
#define EPD_BW_SPI_CMD_SET_RAM_Y_ADDR_COUNTER                     (0x4F)
#define EPD_BW_SPI_CMD_NOP                                        (0xFF)
/**@}*/

/**
 * @name    EPD_BW_SPI display update sequence option flags
 * @brief   Option flags for the EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_2 command.
 *          The flags are executed in the order documented below.
 * @{
 */
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CLOCK       (1<<7)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CP          (1<<6)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_LOAD_TEMP          (1<<5)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_LOAD_LUT           (1<<4)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_INITIAL_DISPLAY    (1<<3)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_PATTERN_DISPLAY    (1<<2)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_DISABLE_CP         (1<<1)
#define EPD_BW_SPI_DISPLAY_UPDATE_OPTION_DISABLE_OSC        (1<<0)
/**@}*/

/**
 * @name    EPD_BW_SPI Waiting estimates
 * @brief   Waiting estimates in milliseconds which are used when the busy pin is not available.
 * @{
 */
#define EPD_BW_SPI_WAIT_UPDATE_FULL 1200
#define EPD_BW_SPI_WAIT_UPDATE_PART 300
#define EPD_BW_SPI_WAIT_ACTIVATION  80
#define EPD_BW_SPI_WAIT_RESET       1
/**@}*/

#ifdef __cplusplus
}
#endif
