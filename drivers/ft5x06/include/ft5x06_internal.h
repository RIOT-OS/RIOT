/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       FT5X06 internal functions
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef FT5X06_INTERNAL_H
#define FT5X06_INTERNAL_H

#include <inttypes.h>
#include "ft5x06.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the maximum touches count of a device, based on its model.
 *
 * @param[in] dev        Device descriptor of the driver
 *
 * @return Maximum touches count
 */
uint8_t ft5x06_get_touches_count_max(const ft5x06_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* FT5X06_INTERNAL_H */
/** @} */
