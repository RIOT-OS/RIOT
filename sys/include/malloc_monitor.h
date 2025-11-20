/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_malloc_monitor
 * @{
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "architecture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtain current heap memory usage.
 *
 * @return      current heap memory usage in bytes
 */
size_t malloc_monitor_get_usage_current(void);

/**
 * @brief Obtain maximum heap memory usage since last call to
 *        @ref malloc_monitor_reset_high_watermark().
 *
 * @return      maximum heap memory usage in bytes
 */
size_t malloc_monitor_get_usage_high_watermark(void);

/**
 * @brief Reset maximum heap memory usage.
 *
 * After calling this function, @ref malloc_monitor_get_usage_high_watermark()
 * will return @ref malloc_monitor_get_usage_current() until further changes
 * to heap memory usage.
 */
void malloc_monitor_reset_high_watermark(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
