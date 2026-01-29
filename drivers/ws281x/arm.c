/*
 * Copyright 2022 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of `ws281x_write_buffer()` for the ARM CPU
 *
 * @author      David VanKampen <david.vankampen@bissell.com>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"

#include "light_ws2812_cortex.h"

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);
    ws2812_sendarray((uint8_t*)buf, size);
}
