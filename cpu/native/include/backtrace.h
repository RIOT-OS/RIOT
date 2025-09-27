/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup backtrace       Stack backtrace (only under native)
 * @ingroup  core_util
 * @brief    Backtrace functionalitry
 * @{
 *
 * If you call the @ref backtrace_print() function a stack backtrace of all return
 * addresses up to @ref BACKTRACE_SIZE will be printed from the point of execution.
 */

/**
 * @file
 * @brief Backtrace functionalitry
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of return addresses to print
 */
#ifndef BACKTRACE_SIZE
#  define BACKTRACE_SIZE  (4U)
#endif

/**
 * @brief   Print up to the last @ref BACKTRACE_SIZE return addresses from call of this
 *          function
 */
void backtrace_print(void);

/**
 * @brief   Print up to the last @ref BACKTRACE_SIZE symbol_names from call of this
 *          function
 */
void backtrace_print_symbols(void);

/**
 * @brief   get the number of stack frames that are printed by print or print_symbols
 *
 */
int backtrace_len(void);

#ifdef __cplusplus
}
#endif

/** @} */
