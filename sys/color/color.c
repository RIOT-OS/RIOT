/*
 * Copyright (C) 2014 - 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_color
 * @{
 *
 * @file
 * @brief       Implementation of color module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 * @author      Simon Brummer <brummer.simon@googlemail.com>
 *
 * @}
 */

#include "color.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void color_rgb2hsv(color_rgb_t *rgb, color_hsv_t *hsv)
{
    float rd, gd, bd, delta, min, max;
    int imax, imin, sector;

    /* catch special case grey first */
    if (rgb->r == rgb->g && rgb->r == rgb->b) {
        hsv->v = (float)rgb->r * (1 / 255.0f);
        hsv->s = 0.0f;
        hsv->h = 0.0f; /* hue might be anything for grey, but it is not uncommon to use 0 */
        return;
    }

    /* normalize RGB colors to the range [0 - 1.0] */
    /* multiplication is often faster than division -> using compile time constant */
    rd = (float)rgb->r * (1 / 255.0f);
    gd = (float)rgb->g * (1 / 255.0f);
    bd = (float)rgb->b * (1 / 255.0f);

    /* find maximum of the three colors and sector */
    /* using the comparing faster integer color value */
    imax = rgb->r;
    max = rd;
    sector = 0;
    if (rgb->g > imax) {
        imax = rgb->g;
        max = gd;
        sector = 1;
    }
    if (rgb->b > imax) {
        imax = rgb->b;
        max = bd;
        sector = 2;
    }
    /* value is maximum*/
    hsv->v = max;

    /* find of minimum the three RGB colors */
    imin = rgb->r;
    min = rd;
    if (rgb->g < imin) {
        imin = rgb->g;
        min = gd;
    }
    if (rgb->b < imin) {
        imin = rgb->b;
        min = bd;
    }
    /* compute delta from value and minimum*/
    delta = hsv->v - min;

    /* find the saturation from value and delta */
    /* special case gray r == g == b ^= min == max */
    hsv->s = delta / max;

    /* compute hue */
    float p = 60.0f / delta;
    switch (sector){
        case 0:
            hsv->h = (gd - bd) * p;
            break;
        case 1:
            hsv->h = 120.0f + (bd - rd) * p;
            break;
        case 2:
            hsv->h = 240.0f + (rd - gd) * p;
            break;
    }
    if (hsv->h < 0.0f) {
        hsv->h += 360.0f;
    }
}

void color_hsv2rgb(color_hsv_t *hsv, color_rgb_t *rgb)
{
    int i;
    float aa, bb, cc, f, h;

    if (hsv->s == 0.0f) {
        rgb->r = (uint8_t)(hsv->v * 255.0f);
        rgb->g = (uint8_t)(hsv->v * 255.0f);
        rgb->b = (uint8_t)(hsv->v * 255.0f);
        return;
    }

    h = (hsv->h == 360.0f) ? 0.0f : hsv->h;
    h /= 60.0f;
    i = (int)h;
    f = h - i;
    aa = hsv->v * (1.0f - hsv->s);
    bb = hsv->v * (1.0f - (hsv->s * f));
    cc = hsv->v * (1.0f - (hsv->s * (1.0f - f)));

    switch (i) {
        case 0:
            rgb->r = (uint8_t)(hsv->v * 255.0f);
            rgb->g = (uint8_t)(cc * 255.0f);
            rgb->b = (uint8_t)(aa * 255.0f);
            break;
        case 1:
            rgb->r = (uint8_t)(bb * 255.0f);
            rgb->g = (uint8_t)(hsv->v * 255.0f);
            rgb->b = (uint8_t)(aa * 255.0f);
            break;
        case 2:
            rgb->r = (uint8_t)(aa * 255.0f);
            rgb->g = (uint8_t)(hsv->v * 255.0f);
            rgb->b = (uint8_t)(cc * 255.0f);
            break;
        case 3:
            rgb->r = (uint8_t)(aa * 255.0f);
            rgb->g = (uint8_t)(bb * 255.0f);
            rgb->b = (uint8_t)(hsv->v * 255.0f);
            break;
        case 4:
            rgb->r = (uint8_t)(cc * 255.0f);
            rgb->g = (uint8_t)(aa * 255.0f);
            rgb->b = (uint8_t)(hsv->v * 255.0f);
            break;
        case 5:
            rgb->r = (uint8_t)(hsv->v * 255.0f);
            rgb->g = (uint8_t)(aa * 255.0f);
            rgb->b = (uint8_t)(bb * 255.0f);
            break;
    }
}

void color_hex2rgb(const uint32_t hex, color_rgb_t *rgb)
{
    rgb->r = ((hex >> 16UL) & 0xFF);
    rgb->g = ((hex >> 8UL) & 0xFF);
    rgb->b = (hex & 0xFF);
}

void color_rgb2hex(const color_rgb_t *rgb, uint32_t *hex)
{
    *hex = (((uint32_t) rgb->r) << 16UL) | (rgb->g << 8UL) | (rgb->b);
}

void color_str2rgb(const char* str, color_rgb_t *rgb)
{
    rgb->r = (((str[0] > '9') ? (str[0] &~ 0x20) - 'A' + 10 : (str[0] - '0')) << 4) | /* R */
             (((str[1] > '9') ? (str[1] &~ 0x20) - 'A' + 10 : (str[1] - '0')) << 0) ; /* R */
    rgb->g = (((str[2] > '9') ? (str[2] &~ 0x20) - 'A' + 10 : (str[2] - '0')) << 4) | /* G */
             (((str[3] > '9') ? (str[3] &~ 0x20) - 'A' + 10 : (str[3] - '0')) << 0) ; /* G */
    rgb->b = (((str[4] > '9') ? (str[4] &~ 0x20) - 'A' + 10 : (str[4] - '0')) << 4) | /* B */
             (((str[5] > '9') ? (str[5] &~ 0x20) - 'A' + 10 : (str[5] - '0')) << 0) ; /* B */
}

void color_rgb2str(const color_rgb_t *rgb, char* str)
{
    uint8_t tmp;

    /* RR */
    tmp = rgb->r >> 4;
    str[0] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->r & 0x0F;
    str[1] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);

    /* GG */
    tmp = rgb->g >> 4;
    str[2] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->g & 0x0F;
    str[3] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);

    /* BB */
    tmp = rgb->b >> 4;
    str[4] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->b & 0x0F;
    str[5] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
}

void color_rgb_complementary(const color_rgb_t *rgb, color_rgb_t *comp_rgb)
{
    uint8_t  max = 0;
    uint8_t  min = 0;
    uint16_t val = 0;

    max = (rgb->r > rgb->g) ? rgb->r : rgb->g;
    max = (rgb->b > max) ? rgb->b : max;
    min = (rgb->r < rgb->g) ? rgb->r : rgb->g;
    min = (rgb->b < min) ? rgb->b : min;
    val = max + min;

    comp_rgb->r = val - rgb->r;
    comp_rgb->g = val - rgb->g;
    comp_rgb->b = val - rgb->b;
}
