/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_aip31068
 * @brief       Internal definitions for the AIP31068 I2C LCD controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Longest execution time is 1.52 ms for 'clear' and 'home' command
 */
#define AIP31068_EXECUTION_TIME_MAX      (1600U)

/**
 * @brief Execution time for writing to RAM is given as 43 µs
 *        Execution time for most commands is given as 37 µs
 */
#define AIP31068_EXECUTION_TIME_DEFAULT  (50U)

#ifdef __cplusplus
}
#endif

/** @} */
