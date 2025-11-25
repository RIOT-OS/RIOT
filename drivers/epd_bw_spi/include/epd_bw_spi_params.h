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
 * @brief    Waveform lookup table for a full display refresh for IL3829.
 */
static const uint8_t epd_bw_spi_il3829_lut_default_full[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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
 * @brief    Configuration for SSD1607 e-paper display controller
 */
#define EPD_BW_SPI_CONTROLLER_SSD1607 EPD_BW_SPI_CONTROLLER_IL3829

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
 * @brief    Configuration for SSD1608 e-paper display controller
 */
#define EPD_BW_SPI_CONTROLLER_SSD1608 { \
            .vcom = 0xA8, \
            .size_x = 240, \
            .size_y = 320, \
            .lut_size = sizeof epd_bw_spi_il3829_lut_default_full, \
            .lut_full = epd_bw_spi_il3829_lut_default_full, \
            .lut_part = epd_bw_spi_il3829_lut_default_part, \
}

#ifndef EPD_BW_SPI_DISPLAY_X
/**
 * @brief Width of the display in pixels.
 */
#define EPD_BW_SPI_DISPLAY_X        (200)
#endif

#ifndef EPD_BW_SPI_DISPLAY_Y
/**
 * @brief Height of the display in pixels.
 */
#define EPD_BW_SPI_DISPLAY_Y        (200)
#endif

#ifndef EPD_BW_SPI_PARAM_SPI
/**
 * @brief SPI device the display is connected to.
 */
#define EPD_BW_SPI_PARAM_SPI        (SPI_DEV(0))
#endif

#ifndef EPD_BW_SPI_PARAM_SPI_CLK
/**
 * @brief SPI device clock speed.
 */
#define EPD_BW_SPI_PARAM_SPI_CLK    (SPI_CLK_5MHZ)
#endif

/**
 * @brief SPI Chip select pin.
 */
#ifndef EPD_BW_SPI_PARAM_CS
#define EPD_BW_SPI_PARAM_CS         (SPI_CS_UNDEF)
#endif

/**
 * @brief Data/command pin of the display.
 */
#ifndef EPD_BW_SPI_PARAM_DC
#define EPD_BW_SPI_PARAM_DC         (GPIO_UNDEF)
#endif

/**
 * @brief Reset pin of the display.
 */
#ifndef EPD_BW_SPI_PARAM_RST
#define EPD_BW_SPI_PARAM_RST        (GPIO_UNDEF)
#endif

#ifndef EPD_BW_SPI_PARAM_BUSY
/**
 * @brief Busy pin of the display.
 */
#define EPD_BW_SPI_PARAM_BUSY       (GPIO_UNDEF)
#endif

#ifndef EPD_BW_SPI_PARAM_BUSY_VAL
/**
 * @brief Width of the display in pixels.
 */
#define EPD_BW_SPI_PARAM_BUSY_VAL   (1)
#endif

#ifndef EPD_BW_SPI_CONTROLLER
/**
 * @brief Display controller. See epd_bw_spi_controller_t.
 */
#define EPD_BW_SPI_CONTROLLER EPD_BW_SPI_CONTROLLER_IL3829
#endif

#ifndef EPD_BW_SPI_ENTRY_MODE
/**
 * @brief Data entry mode. See epd_bw_spi_entry_mode_t.
 */
#define EPD_BW_SPI_ENTRY_MODE EPD_BW_SPI_Y_INC_X_INC
#endif

#ifndef EPD_BW_SPI_PARTIAL_REFRESH_MAX
/**
 * @brief Maximum number of partial refreshes before a full refresh occurs.
 *
 * This is only used with epd_bw_spi_init_auto and @ref drivers_disp_dev.
 */
#define EPD_BW_SPI_PARTIAL_REFRESH_MAX (99)
#endif

#ifndef EPD_BW_SPI_PARAMS
/**
 * @brief Parameters to initialize the display with.
 */
#define EPD_BW_SPI_PARAMS           { .spi = EPD_BW_SPI_PARAM_SPI, \
                                      .spi_clk = EPD_BW_SPI_PARAM_SPI_CLK, \
                                      .cs_pin = EPD_BW_SPI_PARAM_CS, \
                                      .dc_pin = EPD_BW_SPI_PARAM_DC, \
                                      .rst_pin = EPD_BW_SPI_PARAM_RST, \
                                      .busy_pin = EPD_BW_SPI_PARAM_BUSY, \
                                      .busy_value = EPD_BW_SPI_PARAM_BUSY_VAL, \
                                      .controller = EPD_BW_SPI_CONTROLLER, \
                                      .entry_mode = EPD_BW_SPI_ENTRY_MODE, \
                                      .size_x = EPD_BW_SPI_DISPLAY_X, \
                                      .size_y = EPD_BW_SPI_DISPLAY_Y, \
                                      .partial_refresh_max = \
                                          EPD_BW_SPI_PARTIAL_REFRESH_MAX, \
}
#endif

/**
 * @brief   Display driver configuration.
 */
static const epd_bw_spi_params_t epd_bw_spi_params[] =
{
    EPD_BW_SPI_PARAMS,
};

#ifdef __cplusplus
}
#endif
#endif /* EPD_BW_SPI_PARAMS_H */
/**@}*/
