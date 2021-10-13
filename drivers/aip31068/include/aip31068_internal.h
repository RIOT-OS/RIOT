/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_aip31068
 * @brief       Internal definitions for the AIP31068 I2C LCD controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#ifndef AIP31068_INTERNAL_H
#define AIP31068_INTERNAL_H

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

#endif /* AIP31068_INTERNAL_H */
/** @} */
