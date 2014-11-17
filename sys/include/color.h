/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 */

#ifndef __COLOR_H
#define __COLOR_H

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

#ifdef __cplusplus
}
#endif

#endif /* __COLOR_H */
/** @} */
