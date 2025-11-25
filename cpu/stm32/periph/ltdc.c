/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Low-level LTDC (LCD-TFT Display controller) driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdalign.h>
#include <string.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#if IS_USED(MODULE_DISP_DEV)
#include "disp_dev.h"
#endif

#define ENABLE_DEBUG        0
#include "debug.h"

#ifndef LCD_SCREEN_WIDTH
#define LCD_SCREEN_WIDTH            480
#endif

#ifndef LCD_SCREEN_HEIGHT
#define LCD_SCREEN_HEIGHT           272
#endif

#define LTDC_BLENDING_FACTOR1_CA    0x00000400U
#define LTDC_BLENDING_FACTOR2_CA    0x00000005U

typedef struct {
    uint32_t hsync;
    uint32_t vsync;
    uint32_t acc_hbp;
    uint32_t acc_vbp;
    uint32_t acc_active_h;
    uint32_t acc_active_w;
    uint32_t height;
    uint32_t width;
} ltdc_display_conf_t;

/* allocate array of 16bit pixels for the framebuffer (TODO: use external SRAM via FMC) */
static uint16_t _ltdc_frame_buffer[LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT];

static void _init_gpio(void)
{
    gpio_init(ltdc_config.clk_pin.pin, GPIO_OUT);
    gpio_init_af(ltdc_config.clk_pin.pin, ltdc_config.clk_pin.af);
    gpio_init(ltdc_config.de_pin.pin, GPIO_OUT);
    gpio_init_af(ltdc_config.de_pin.pin, ltdc_config.de_pin.af);
    gpio_init(ltdc_config.hsync_pin.pin, GPIO_OUT);
    gpio_init_af(ltdc_config.hsync_pin.pin, ltdc_config.hsync_pin.af);
    gpio_init(ltdc_config.vsync_pin.pin, GPIO_OUT);
    gpio_init_af(ltdc_config.vsync_pin.pin, ltdc_config.vsync_pin.af);

    for (uint8_t pin = 0; pin < 8; ++pin) {
        if (!gpio_is_valid(ltdc_config.r_pin[pin].pin)) {
            continue;
        }
        gpio_init(ltdc_config.r_pin[pin].pin, GPIO_OUT);
        gpio_init_af(ltdc_config.r_pin[pin].pin, ltdc_config.r_pin[pin].af);
        gpio_init(ltdc_config.g_pin[pin].pin, GPIO_OUT);
        gpio_init_af(ltdc_config.g_pin[pin].pin, ltdc_config.g_pin[pin].af);
        gpio_init(ltdc_config.b_pin[pin].pin, GPIO_OUT);
        gpio_init_af(ltdc_config.b_pin[pin].pin, ltdc_config.b_pin[pin].af);
    }
}

static void _configure_ltdc(void)
{
    uint32_t tmp;

    const ltdc_display_conf_t ltdc_display_config = {
        .hsync = ltdc_config.hsync - 1,
        .vsync = ltdc_config.vsync - 1,
        .acc_hbp = ltdc_config.hsync + ltdc_config.hbp - 1,
        .acc_vbp = ltdc_config.vsync + ltdc_config.vbp - 1,
        .acc_active_h = LCD_SCREEN_HEIGHT + ltdc_config.vsync + ltdc_config.vbp - 1,
        .acc_active_w = LCD_SCREEN_WIDTH + ltdc_config.hsync + ltdc_config.hbp - 1,
        .height = LCD_SCREEN_HEIGHT + ltdc_config.vsync + ltdc_config.vbp + ltdc_config.vfp - 1,
        .width = LCD_SCREEN_WIDTH + ltdc_config.hsync + ltdc_config.hbp + ltdc_config.hfp - 1,
    };

    /* Configure the HS, VS, DE and PC polarity: all active low*/
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);

    /* Set Synchronization size */
    LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
    tmp = (ltdc_display_config.hsync << 16U);
    LTDC->SSCR |= (tmp | ltdc_display_config.vsync);

    /* Set Accumulated Back porch */
    LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
    tmp = (ltdc_display_config.acc_hbp << 16U);
    LTDC->BPCR |= (tmp | ltdc_display_config.acc_vbp);

    /* Set Accumulated Active Width */
    LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
    tmp = (ltdc_display_config.acc_active_w << 16U);
    LTDC->AWCR |= (tmp | ltdc_display_config.acc_active_h);

    /* Set Total Width */
    LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
    tmp = (ltdc_display_config.width << 16U);
    LTDC->TWCR |= (tmp | ltdc_display_config.height);

    /* Set the background color value: black */
    LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);

    /* Enable LTDC */
    LTDC->GCR |= LTDC_GCR_LTDCEN;
}

static void _configure_ltdc_layer(void)
{
    uint32_t tmp;

    /* Configure the horizontal start and stop position */
    tmp = ((LCD_SCREEN_WIDTH + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U);
    LTDC_Layer1->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
    LTDC_Layer1->WHPCR = ((0 + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | tmp);

    /* Configure the vertical start and stop position */
    tmp = ((LCD_SCREEN_HEIGHT + (LTDC->BPCR & LTDC_BPCR_AVBP)) << 16U);
    LTDC_Layer1->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
    LTDC_Layer1->WVPCR  = ((0 + (LTDC->BPCR & LTDC_BPCR_AVBP) + 1U) | tmp);

    /* Set the pixel format: RGB565 (16bit) */
    LTDC_Layer1->PFCR &= ~(LTDC_LxPFCR_PF);
    LTDC_Layer1->PFCR = 2;

    /* Configure the default color values: all black */
    LTDC_Layer1->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);

    /* Set the constant alpha value: fully opaque */
    LTDC_Layer1->CACR &= ~(LTDC_LxCACR_CONSTA);
    LTDC_Layer1->CACR = 255;

    /* Set the blending factors */
    LTDC_Layer1->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
    LTDC_Layer1->BFCR = (LTDC_BLENDING_FACTOR1_CA | LTDC_BLENDING_FACTOR2_CA);

    /* Configure the color frame buffer start address */
    LTDC_Layer1->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
    LTDC_Layer1->CFBAR = (uint32_t)_ltdc_frame_buffer;

    /* Configure the color frame buffer pitch in byte */
    LTDC_Layer1->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
    LTDC_Layer1->CFBLR  = (((LCD_SCREEN_WIDTH * 2) << 16U) | ((LCD_SCREEN_WIDTH * 2) + 3U));
    /* Configure the frame buffer line number */
    LTDC_Layer1->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
    LTDC_Layer1->CFBLNR  = LCD_SCREEN_HEIGHT;

    /* Enable LTDC_Layer by setting LEN bit */
    LTDC_Layer1->CR |= (uint32_t)LTDC_LxCR_LEN;
}

void ltdc_init(void)
{
    DEBUG("[ltdc] init: initializing device\n");

    periph_clk_en(ltdc_config.bus, ltdc_config.rcc_mask);

    _init_gpio();
    _configure_ltdc();
    _configure_ltdc_layer();
}

void ltdc_clear(void)
{
    memset(_ltdc_frame_buffer, 0, LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT * sizeof(uint16_t));
    LTDC->SRCR = LTDC_SRCR_IMR;
}

void ltdc_map(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t *color)
{
    for (uint16_t y = y1; y <= y2; y++) {
        for (uint16_t x = x1; x <= x2; x++) {
            *(_ltdc_frame_buffer + (x + y * LCD_SCREEN_WIDTH)) = *color++;
        }
    }
    LTDC->SRCR = LTDC_SRCR_IMR;
}

void ltdc_fill(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t color)
{
    for (uint16_t y = y1; y <= y2; y++) {
        for (uint16_t x = x1; x <= x2; x++) {
            *(_ltdc_frame_buffer + (x + y * LCD_SCREEN_WIDTH)) = color;
        }
    }

    LTDC->SRCR = LTDC_SRCR_IMR;
}

#if IS_USED(MODULE_DISP_DEV)
static void _ltdc_map(const disp_dev_t *disp_dev, const disp_dev_area_t *area, const uint16_t *color)
{
    (void)disp_dev;
    ltdc_map(area->x1, area->x2, area->y1, area->y2, color);
}

static uint16_t _ltdc_height(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return LCD_SCREEN_HEIGHT;
}

static uint16_t _ltdc_width(const disp_dev_t *disp_dev)
{
    (void)disp_dev;

    return LCD_SCREEN_WIDTH;
}

static uint8_t _ltdc_color_depth(const disp_dev_t *disp_dev)
{
    (void)disp_dev;
    return 16;
}

static void _ltdc_set_invert(const disp_dev_t *disp_dev, bool invert)
{
    (void)disp_dev;
    (void)invert;
}

const disp_dev_driver_t stm32_ltdc_disp_dev_driver = {
    .map            = _ltdc_map,
    .height         = _ltdc_height,
    .width          = _ltdc_width,
    .color_depth    = _ltdc_color_depth,
    .set_invert     = _ltdc_set_invert,
};

#endif
