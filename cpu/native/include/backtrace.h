/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    backtrace       Stack backtrace (only under native)
 * @ingroup     core_util
 * @brief       Backtrace functionalitry
 *
 * If you call the @ref backtrace_print() function a stack backtrace of all return
 * addresses up to @ref BACKTRACE_SIZE will be printed from the point of execution.
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef BACKTRACE_H
#define BACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of return addresses to print
 */
#ifndef BACKTRACE_SIZE
#define BACKTRACE_SIZE  (4U)
#endif

/**
 * @brief   Print the last @ref BACKTRACE_SIZE return addresses from call of this
 *          function
 */
void backtrace_print(void);

#ifdef __cplusplus
}
#endif

#endif /* BACKTRACE_H */
/** @} */
