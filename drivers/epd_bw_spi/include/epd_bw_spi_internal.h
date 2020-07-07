/*
 * Copyright (C) 2019 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef EPD_BW_SPI_INTERNAL_H
#define EPD_BW_SPI_INTERNAL_H

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    EPD_BW_SPI SPI commands
 * @brief   Commands used for controlling EPD displays.
 *          These are surprisingly portable.
 * @{
 */
#define EPD_BW_SPI_CMD_DRIVER_OUTPUT_CONTROL (0x01)
#define EPD_BW_SPI_CMD_GATE_DRIVING_VOLTAGE_CONTROL (0x03)                  /* unused */
#define EPD_BW_SPI_CMD_SOURCE_DRIVING_VOLTAGE_CONTROL (0x04)                /* unused */
#define EPD_BW_SPI_CMD_DISPLAY_CONTROL (0x07)                               /* unused */
#define EPD_BW_SPI_CMD_GATE_AND_SOURCE_NON_OVERLAP_PERIOD_CONTROL (0x0B)    /* unused */
#define EPD_BW_SPI_CMD_BOOSTER_SOFT_START_CONTROL (0x0C)
#define EPD_BW_SPI_CMD_GATE_SCAN_START_POSITION (0x0F)                      /* unused */
#define EPD_BW_SPI_CMD_DEEP_SLEEP_MODE (0x10)
#define EPD_BW_SPI_CMD_DATA_ENTRY_MODE_SETTING (0x11)
#define EPD_BW_SPI_CMD_SWRESET (0x12)
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE (0x1A)      /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_READ (0x1B)       /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_WRITE_CMD (0x1C)  /* unused */
#define EPD_BW_SPI_CMD_TEMPERATURE_SENSOR_CONTROL_LOAD (0x1D)       /* unused */
#define EPD_BW_SPI_CMD_MASTER_ACTIVATION (0x20)
#define EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_1 (0x21)              /* unused */
#define EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_2 (0x22)
#define EPD_BW_SPI_CMD_WRITE_RAM (0x24)
#define EPD_BW_SPI_CMD_READ_RAM (0x25)              /* unused */
#define EPD_BW_SPI_CMD_VCOM_SENSE (0x28)            /* unused */
#define EPD_BW_SPI_CMD_VCOM_SENSE_DURATION (0x29)   /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_VCOM_OTP (0x2A)      /* unused */
#define EPD_BW_SPI_CMD_WRITE_VCOM_REGISTER (0x2C)
#define EPD_BW_SPI_CMD_READ_OTP_REGISTERS (0x2D)    /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_WS_OTP (0x30)        /* unused */
#define EPD_BW_SPI_CMD_WRITE_LUT_REGISTER (0x32)
#define EPD_BW_SPI_CMD_READ_LUT_REGISTER (0x33)     /* unused */
#define EPD_BW_SPI_CMD_PROGRAM_OTP_SELECTION (0x36) /* unused */
#define EPD_BW_SPI_CMD_OTP_SELECTION_CONTROL (0x37) /* unused */
#define EPD_BW_SPI_CMD_SET_DUMMY_LINE_PERIOD (0x3A)
#define EPD_BW_SPI_CMD_SET_GATE_LINE_WIDTH (0x3B)
#define EPD_BW_SPI_CMD_BORDER_WAVEFORM_CONTROL (0x3C) /* unused */
#define EPD_BW_SPI_CMD_SET_RAM_X (0x44)
#define EPD_BW_SPI_CMD_SET_RAM_Y (0x45)
#define EPD_BW_SPI_CMD_SET_RAM_X_ADDR_COUNTER (0x4E)
#define EPD_BW_SPI_CMD_SET_RAM_Y_ADDR_COUNTER (0x4F)
#define EPD_BW_SPI_CMD_NOP (0xFF)
/**@}*/

/**
 * @name    EPD_BW_SPI Waiting estimates in microseconds
 * @brief   These waiting estimates are used when the busy pin is not available.
 * @{
 */
#define EPD_BW_SPI_WAIT_UPDATE_FULL (1200 * US_PER_MS)
#define EPD_BW_SPI_WAIT_UPDATE_PART (300 * US_PER_MS)
#define EPD_BW_SPI_WAIT_ACTIVATION  (80 * US_PER_MS)
#define EPD_BW_SPI_WAIT_RESET       (1 * US_PER_MS)
/**@}*/

/**
 * @brief   Send a SPI display command with data.
 *
 * This function requires that `spi_acquire()` has been called.
 *
 * @param[in] p         SPI display parameters.
 * @param[in] cmd       Command to execute.
 * @param[in] cont      Continue with data after this command.
 */
void epd_bw_spi_cmd_start(epd_bw_spi_params_t *p, uint8_t cmd, bool cont);

/**
 * @brief   Send a SPI display command with data.
 *
 * @param[in] p         SPI display parameters.
 * @param[in] cmd       Command to execute.
 * @param[in] params    Command parameters to send.
 * @param[in] plen      Size of the command parameters.
 */
void epd_bw_spi_write_cmd(epd_bw_spi_params_t *p, uint8_t cmd,
                          const uint8_t *params, size_t plen);

/**
 * @brief   Send a SPI display command and store the returned data.
 *
 * @param[in] p         SPI display parameters.
 * @param[in] cmd       Command to execute.
 * @param[in] params    Command response to read.
 * @param[in] plen      Size of the command response.
 */
void epd_bw_spi_read_cmd(epd_bw_spi_params_t *p, uint8_t cmd, uint8_t *params,
                         size_t plen);

/**
 * @brief   Wait for the busy pin or a predetermined amount of time.
 *
 * @param[in] p         SPI display parameters.
 * @param[in] usec      Number of µs to wait if the busy pin is undefined.
 */
void epd_bw_spi_wait(epd_bw_spi_params_t *p, uint32_t usec);

#ifdef __cplusplus
}
#endif
#endif /* EPD_BW_SPI_INTERNAL_H */
