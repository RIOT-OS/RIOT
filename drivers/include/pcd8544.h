/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_pcd8544 PCD8544 LCD driver
 * @ingroup     drivers_display
 * @brief       Driver for PCD8544 LCD displays
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the PCD8544 LCD driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    Definition of display dimensions
 * @{
 */
#define PCD8544_RES_X                   (84U)   /**< pixels per row */
#define PCD8544_RES_Y                   (48U)   /**< pixels per column */
#define PCD8544_COLS                    (14U)   /**< characters per row */
#define PCD8544_ROWS                    (6U)    /**< characters per column */
/** @} */

/**
 * @name    Default values for temperature compensation and contrast
 * @{
 */
#define PCD8544_DEFAULT_CONTRAST        (45U)
#define PCD8544_DEFAULT_BIAS            (3U)
#define PCD8544_DEFAULT_TEMPCOEF        (0U)
/** @} */

/**
 * @brief   PCD8544 device descriptor
 */
typedef struct {
    spi_t spi;          /**< SPI bus the display is connected to */
    gpio_t cs;          /**< chip-select pin, low: active */
    gpio_t reset;       /**< reset pin, low: active */
    gpio_t mode;        /**< mode pin: low: cmd mode, high: data mode */
    uint8_t inverted;   /**< internal flag to keep track of inversion state */
} pcd8544_t;

/**
 * @brief   Initialize the given display
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] spi           SPI bus the display is connected to
 * @param[in] cs            GPIO pin that is connected to the CS pin
 * @param[in] reset         GPIO pin that is connected to the RESET pin
 * @param[in] mode          GPIO pin that is connected to the MODE pin
 *
 * @return      0 on success
 * @return      <0 on error
 */
int pcd8544_init(pcd8544_t *dev, spi_t spi, gpio_t cs,
                 gpio_t reset, gpio_t mode);

/**
 * @brief   Set the contrast for the given display
 *
 * @note    A contrast value of 45 yields good results for 3V3
 *
 * @param[in] dev           display device descriptor
 * @param[in] contrast      targeted contrast value [0 - 127]
 */
void pcd8544_set_contrast(const pcd8544_t *dev, uint8_t contrast);

/**
 * @brief   Set the temperature coefficient for the given display
 *
 * @note    Look at the datasheet for more information
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] coef          temperature coefficient to use [0 - 3]
 */
void pcd8544_set_tempcoef(const pcd8544_t *dev, uint8_t coef);

/**
 * @brief   Set the internal BIAS for the given display
 *
 * @note    Look at the datasheet for more information
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] bias          the BIAS to use [0 - 7]
 *
 */
void pcd8544_set_bias(const pcd8544_t *dev, uint8_t bias);

/**
 * @brief   Write an image to memory of the given display
 *
 * The image must be given as a char array with 504 elements. Each bit in the
 * array represents one pixel on the display. Each byte in the array contains
 * 8 stacked pixels, from top to bottom. So byte[0] contains the pixels from
 * (0,0) to (0,7), byte[1] (1,0) to (1,7) and byte[503] the pixels from
 * (83,40) to (83,47) -> see the 'horizontal addressing' section in the
 * datasheet.
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] img           uint8_t array with image data (must be of size := 504)
 */
void pcd8544_write_img(const pcd8544_t *dev, const uint8_t img[]);

/**
 * @brief   Write a single ASCII character to the display
 *
 * The position of the character is specified in columns (x) and rows (y)
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] x             column to put the character [0 - 13]
 * @param[in] y             row to put the character [0 - 5]
 * @param[in] c             ASCII code for the character to write
 */
void pcd8544_write_c(const pcd8544_t *dev, uint8_t x, uint8_t y, const char c);

/**
 * @brief   Write a string to a given position on the display
 *
 * This function prints a given string to the given position on the display. The
 * position is given in terms of columns (x) and rows (y). If a string does not
 * fit completely in the given position (it overflows its row), the overflowing
 * part of the string is cut off.
 *
 * @param[in] dev           device descriptor of display to use
 * @param[in] x             starting column of the string [0 - 13]
 * @param[in] y             row to write the string to [0 - 5]
 * @param[in] str           string to write to the display
 */
void pcd8544_write_s(const pcd8544_t *dev, uint8_t x, uint8_t y, const char* str);

/**
 * @brief   Clear the current display (clear the display memory)
 *
 * @param[in] dev           device descriptor of display to use
 */
void pcd8544_clear(const pcd8544_t *dev);

/**
 * @brief   Invert the display (toggles dark and bright pixels)
 *
 * @param[in] dev           device descriptor of display to use
 */
void pcd8544_invert(pcd8544_t *dev);

/**
 * @brief   Get the current inversion status of the display
 *
 * @param[in] dev           device descriptor of display to use
 *
 * @return      0 -> display is not inverted
 * @return      1 -> display is inverted
 */
int pcd8544_is_inverted(const pcd8544_t *dev);

/**
 * @brief   Power on the display
 *
 * @param[in] dev           device descriptor of display to use
 */
void pcd8544_poweron(const pcd8544_t *dev);

/**
 * @brief   Poser off the display
 *
 * @param[in] dev           device descriptor of display to use
 */
void pcd8544_poweroff(const pcd8544_t *dev);

/**
 * @brief   I wonder what this does -> find out!
 *
 * @param[in] dev           device descriptor of display to use
 */
void pcd8544_riot(const pcd8544_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
