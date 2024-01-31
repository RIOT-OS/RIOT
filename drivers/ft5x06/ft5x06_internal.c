/*
 * Copyright (c) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       Implementation of internal functions for ft5x06
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */
#include <inttypes.h>

#include "ft5x06.h"
#include "ft5x06_internal.h"

uint8_t ft5x06_get_touches_count_max(const ft5x06_t *dev) {
    if (dev->params->type == FT5X06_TYPE_FT6X06 || dev->params->type == FT5X06_TYPE_FT6X36) {
        return FT6XX6_TOUCHES_COUNT_MAX;
    }
    else {
        return FT5X06_TOUCHES_COUNT_MAX;
    }
}
