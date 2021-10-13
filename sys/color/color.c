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
    float rd, gd, bd, delta;

    /* norm RGB colors to the range [0 - 1.0] */
    rd = (float)rgb->r / 255.0f;
    gd = (float)rgb->g / 255.0f;
    bd = (float)rgb->b / 255.0f;

    /* find value as maximum of the three colors */
    if (rd >= gd) {
        hsv->v = (rd >= bd) ? rd : bd;
    }
    else {
        hsv->v = (gd >= bd) ? gd : bd;
    }

    /* compute delta from value and minimum of the three RGB colors */
    if (rd <= gd) {
        delta = (rd <= bd) ? (hsv->v - rd) : (hsv->v - bd);
    }
    else {
        delta = (gd <= bd) ? (hsv->v - gd) : (hsv->v - bd);
    }

    /* find the saturation from value and delta */
    hsv->s = (hsv->v != 0.0f) ? (delta / hsv->v) : 0.0f;

    /* compute hue */
    hsv->h = 0.0f;
    if (hsv->s != 0.0f) {
        float rc, gc, bc;

        rc = (hsv->v - rd) / delta;
        gc = (hsv->v - gd) / delta;
        bc = (hsv->v - bd) / delta;

        if (rd == hsv->v) {
            hsv->h = bc - gc;
        }
        else if (gd == hsv->v) {
            hsv->h = 2.0f + rc - bc;
        }
        else {
            hsv->h = 4.0f + gc - rc;
        }
        hsv->h *= 60.0f;
        if (hsv->h < 0.0f) {
            hsv->h += 360.0f;
        }
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
