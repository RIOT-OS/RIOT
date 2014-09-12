/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 *
 * @}
 */

#include "color.h"


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
    hsv->s = (hsv->v != 0.0f) ? (delta / hsv->v) : 0x0f;

    /* compute hue */
    hsv->h = 0.0f;
    if (hsv->s != 0.0) {
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
            hsv->h += 360.0;
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

    h = (hsv->h == 360.0f) ? 0.0 : hsv->h;
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
