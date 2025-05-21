/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           LTDC CPU specific definitions for the STM32 family
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdint.h>

#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LTDC GPIO configuration
 */
typedef struct {
    gpio_t pin;                     /**< GPIO pin */
    gpio_af_t af;                   /**< Alternate function */
} ltdc_gpio_t;

/**
 * @brief   LTDC Peripheral configuration
 */
typedef struct {
    uint8_t bus;                    /**< APB bus */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    ltdc_gpio_t clk_pin;            /**< CLK pin */
    ltdc_gpio_t de_pin;             /**< Data enable pin */
    ltdc_gpio_t hsync_pin;          /**< Horizontal synchronization pin */
    ltdc_gpio_t vsync_pin;          /**< Vertical synchronization pin */
    ltdc_gpio_t r_pin[8];           /**< Red color pins */
    ltdc_gpio_t g_pin[8];           /**< Green color pins */
    ltdc_gpio_t b_pin[8];           /**< Blue color pins */
    uint8_t hsync;                  /**< Horizontal synchronization */
    uint8_t vsync;                  /**< Vertical synchronization */
    uint8_t hbp;                    /**< Horizontal back porch */
    uint8_t hfp;                    /**< Horizontal front porch */
    uint8_t vbp;                    /**< Vertical back porch */
    uint8_t vfp;                    /**< Vertical front porch */
} ltdc_conf_t;

/**
 * @brief   Initialize the LTDC (LCD-TFT Display Controller) peripheral
 */
void ltdc_init(void);

/**
 * @brief   Clear the LTDC display
 */
void ltdc_clear(void);

/**
 * @brief   Map a buffer of RGB565 (16bit depth) colors to the display
 *
 * @param[in]  x1       horizontal start position
 * @param[in]  x2       horizontal end position (included)
 * @param[in]  y1       vertical start position
 * @param[in]  y2       vertical end position (included)
 * @param[in]  color    the color buffer
 */
void ltdc_map(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t *color);

/**
 * @brief   Fill a region of the display with the same color
 *
 * @param[in]  x1       horizontal start position
 * @param[in]  x2       horizontal end position (included)
 * @param[in]  y1       vertical start position
 * @param[in]  y2       vertical end position (included)
 * @param[in]  color    the color value in RGB565 format (16bit depth)
 */
void ltdc_fill(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t color);

#ifdef __cplusplus
}
#endif

/** @} */
