/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 * @file
 * @brief       FT5X06 internal functions
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

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

/** @} */
