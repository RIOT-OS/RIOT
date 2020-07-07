/*
 * Copyright (C) 2019 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_epd_bw_spi
 *
 * @{
 * @file
 * @brief       Default configuration for epd_bw_spi
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#ifndef EPD_BW_SPI_PARAMS_H
#define EPD_BW_SPI_PARAMS_H

#include "board.h"
#include "epd_bw_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Set default configuration parameters for a SPI e-Paper display
 * @{
 */
#ifndef EPD_BW_SPI_PARAM_SPI
#define EPD_BW_SPI_PARAM_SPI        (SPI_DEV(0))
#endif
#ifndef EPD_BW_SPI_PARAM_SPI_CLK
#define EPD_BW_SPI_PARAM_SPI_CLK    (SPI_CLK_5MHZ)
#endif
#ifndef EPD_BW_SPI_PARAM_CS
#define EPD_BW_SPI_PARAM_CS         (GPIO_UNDEF)
#endif
#ifndef EPD_BW_SPI_PARAM_DC
#define EPD_BW_SPI_PARAM_DC         (GPIO_UNDEF)
#endif
#ifndef EPD_BW_SPI_PARAM_RST
#define EPD_BW_SPI_PARAM_RST        (GPIO_UNDEF)
#endif
#ifndef EPD_BW_SPI_PARAM_BUSY
#define EPD_BW_SPI_PARAM_BUSY       (GPIO_UNDEF)
#endif

#ifndef EPD_BW_SPI_PARAMS
#define EPD_BW_SPI_PARAMS           { .spi = EPD_BW_SPI_PARAM_SPI, \
                                      .spi_clk = EPD_BW_SPI_PARAM_SPI_CLK, \
                                      .cs_pin = EPD_BW_SPI_PARAM_CS, \
                                      .dc_pin = EPD_BW_SPI_PARAM_DC, \
                                      .rst_pin = EPD_BW_SPI_PARAM_RST, \
                                      .busy_pin = EPD_BW_SPI_PARAM_BUSY, \
}
#endif
/**@}*/

/**
 * @brief   SPI configuration of display
 */
static const epd_bw_spi_params_t epd_bw_spi_params[] =
{
    EPD_BW_SPI_PARAMS,
};

/**
 * @brief    Waveform lookup table for a full display refresh for IL3829.
 */
static const uint8_t epd_bw_spi_il3829_lut_default_full[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/**
 * @brief    Waveform lookup table for a partial display refresh for IL3829.
 */
static const uint8_t epd_bw_spi_il3829_lut_default_part[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/**
 * @brief    Configuration for IL3829 e-paper display controller
 */
#define EPD_BW_SPI_CONTROLLER_IL3829 { \
        .vcom = 0xA8, \
        .size_x = 200, \
        .size_y = 300, \
        .lut_size = sizeof epd_bw_spi_il3829_lut_default_full, \
        .lut_full = epd_bw_spi_il3829_lut_default_full, \
        .lut_part = epd_bw_spi_il3829_lut_default_part, \
}

/**
 * @brief    Configuration for SSD1673 e-paper display controller
 */
#define EPD_BW_SPI_CONTROLLER_SSD1673 { \
        .vcom = 0xA8, \
        .size_x = 150, \
        .size_y = 250, \
        .lut_size = sizeof epd_bw_spi_il3829_lut_default_full, \
        .lut_full = epd_bw_spi_il3829_lut_default_full, \
        .lut_part = epd_bw_spi_il3829_lut_default_part, \
}

/**
 * @brief    Default controller definition
 */
#ifndef EPD_BW_SPI_CONTROLLER
#define EPD_BW_SPI_CONTROLLER EPD_BW_SPI_CONTROLLER_IL3829
#endif

/**
 * @brief   SPI configuration of display
 */
static const epd_bw_spi_controller_t epd_bw_spi_controller[] =
{
    EPD_BW_SPI_CONTROLLER,
};

#ifdef __cplusplus
}
#endif
#endif /* EPD_BW_SPI_PARAMS_H */
