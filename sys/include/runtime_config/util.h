/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_runtime_config_util Runtime configuration utilities
 * @ingroup     sys_runtime_config
 * @brief       Runtime configuration "Util" module providing utility functions
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "runtime_config.h"

/**
 * @brief Specifies the number of digits after decimal point when converting
 *        float or double runtime configuration values to strings.
 */
#ifndef CONFIG_RUNTIME_CONFIG_UTIL_FORMAT_FLOAT_PRECISION
#  define CONFIG_RUNTIME_CONFIG_UTIL_FORMAT_FLOAT_PRECISION 6
#endif

/**
 * @brief Convenience function to parse a configuration parameter value from
 * a string.
 *
 * @param[in] src Pointer of the input value. Must not be NULL.
 * @param[out] dest Pointer to the output buffer. Must not be NULL.
 * @param[in] dest_len Length of @p dest.
 * @param[in] dest_type Type of the output value.
 *
 * @return 0 on success, non-zero on failure.
 */
int runtime_config_util_convert_str_to_value(
    const char *src,
    void *dest,
    const size_t dest_len,
    const runtime_config_type_t dest_type);

/**
 * @brief Convenience function to transform a configuration parameter value into
 * its representation as a string.
 *
 * @param[in] src Pointer to the value to be converted.
 * @param[out] dest Buffer to store the output string.
 * @param[in] dest_len Length of @p buf.
 *
 * @return Length of string value on success, non-zero on failure.
 */
int runtime_config_util_convert_value_to_str(
    const runtime_config_value_t *src,
    char *dest,
    const size_t dest_len);

#ifdef __cplusplus
}
#endif

/** @} */
