/*
 * Copyright (C) 2014 - 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_color Color
 * @ingroup     sys
 * @brief       The color sys module supports handling RGB and HSV color
 * @{
 *
 * @file
 * @brief       Headers for the color handling module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 * @author      Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data-structure describing a RGB color
 */
typedef struct {
    uint8_t r;          /**< red value      [0 - 255] */
    uint8_t g;          /**< green value    [0 - 255] */
    uint8_t b;          /**< blue value     [0 - 255] */
} color_rgb_t;

/**
 * @brief RGBA color value
 */
typedef struct {
    color_rgb_t color;  /**< RGB value */
    uint8_t alpha;      /**< alpha value [0 - 255] */
} color_rgba_t;

/**
 * @brief Data-structure for holding HSV colors
 */
typedef struct {
    float h;            /**< hue value        [0.0 - 360.0] */
    float s;            /**< saturation value [0.0 - 1.0] */
    float v;            /**< value            [0.0 - 1.0] */
} color_hsv_t;

/**
 * @brief Convert RGB color to HSV color
 *
 * @param[in] rgb       Input color encoded in RGB space
 * @param[out] hsv      Output color encoded in HSV space
 */
void color_rgb2hsv(color_rgb_t *rgb, color_hsv_t *hsv);

/**
 * @brief Convert HSV color to RGB color
 *
 * @param[in] hsv       Input color encoded in HSV space
 * @param[out] rgb      Output color encoded in RGB space
 */
void color_hsv2rgb(color_hsv_t *hsv, color_rgb_t *rgb);

/**
 * @brief Convert a @p hex value of the form 0x00RRGGBB to an RGB color struct
 *
 * @note                the two most significant bytes of @p hex will be ignored
 *
 * @param[in] hex       Input color encoded in hex
 * @param[out] rgb      Output color encoded in RGB space
 */
void color_hex2rgb(const uint32_t hex, color_rgb_t *rgb);

/**
 * @brief Convert a @p rgb struct to a @p hex value of the form 0x00RRGGBB
 *
 * @note                the two most significant bytes of @p hex will be 0
 *
 * @param[in] rgb       Input color encoded in RGB space
 * @param[out] hex      Output color encoded in hex
 */
void color_rgb2hex(const color_rgb_t *rgb, uint32_t *hex);

/**
 * @brief Convert a hex color string of the form 'RRGGBB' to a color_rgb_t struct
 *
 * @note                @p str MUST contain only hexadecimal digits.
 *                      Expect unexpected behaviour, otherwise.
 *
 * @param[in] str       Input color encoded as string of the form 'RRGGBB'
 * @param[out] color    Output color encoded in RGB space
 */
void color_str2rgb(const char *str, color_rgb_t *color);

/**
 * @brief Convert a color_rgb_t struct to a hex color string of the form 'RRGGBB'
 *
 * @note                @p str MUST be big enough to hold 6 characters
 *
 * @param[in] rgb       Input color encoded in RGB space
 * @param[out] str      Output color encoded as string of the form 'RRGGBB'
 */
void color_rgb2str(const color_rgb_t *rgb, char *str);

/**
 * @brief Invert a given rgb color
 *
 * @pre                 ((rgb != NULL) && (inv_rgb != NULL))
 *
 * @param[in] rgb       Input rgb color, that should be converted. Must be NOT NULL
 * @param[out] inv_rgb  Output rgb color, result of the conversion. Must be NOT NULL
 */
static inline void color_rgb_invert(const color_rgb_t *rgb, color_rgb_t *inv_rgb)
{
    inv_rgb->r = rgb->r ^ 0xFF;
    inv_rgb->g = rgb->g ^ 0xFF;
    inv_rgb->b = rgb->b ^ 0xFF;
}

/**
 * @brief Shifts a given rgb color to change it's brightness
 *
 * @pre                 ((rgb != NULL) && (out != NULL))
 *
 * @param[in] rgb       Input rgb color, that should be shifted. Must be NOT NULL
 * @param[out] out      Output rgb color, result of the shift. Must be NOT NULL
 * @param[in] shift     Amount by which the color components should be shifted.
 *                      May be positive (shift to left) or negative (shift to right).
 */
static inline void color_rgb_shift(const color_rgb_t *rgb, color_rgb_t *out, int8_t shift)
{
    if (shift > 0) {
        out->r = rgb->r << shift;
        out->g = rgb->g << shift;
        out->b = rgb->b << shift;
    } else {
        out->r = rgb->r >> -shift;
        out->g = rgb->g >> -shift;
        out->b = rgb->b >> -shift;
    }
}

/**
 * @brief Change the brightness of a RGB color by multiplying it with a set factor.
 *
 * @pre                 ((rgb != NULL) && (out != NULL))
 *
 * @param[in] rgb       Input rgb color, that should be multiplied. Must be NOT NULL
 * @param[out] out      Output rgb color. Must be NOT NULL
 * @param[in] level     New brightness level. 255 = Full Brightness, 0 = Off.
 */
static inline void color_rgb_set_brightness(const color_rgb_t *rgb, color_rgb_t *out, uint8_t level)
{
    out->r = ((unsigned)rgb->r * level + 128) >> 8;
    out->g = ((unsigned)rgb->g * level + 128) >> 8;
    out->b = ((unsigned)rgb->b * level + 128) >> 8;
}

/**
 * @brief Calculate the complementary color of a given rgb color.
 *
 * @note                Complementary color calculation according to adobe illustator calculations.
 *                      See https://helpx.adobe.com/illustrator/using/adjusting-colors.html
 *
 * @pre                 ((rgb != NULL) && (comp_rgb != NULL))
 *
 * @param[in] rgb       Input rgb color. Must be NOT NULL
 * @param[out] comp_rgb Output rgb color, result of the complementary color calculation. Must be NOT NULL
 */
void color_rgb_complementary(const color_rgb_t *rgb, color_rgb_t *comp_rgb);

#ifdef __cplusplus
}
#endif

#endif /* COLOR_H */
/** @} */
