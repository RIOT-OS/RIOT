/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    trace       Stack traceback (only under native)
 * @ingroup     core_util
 * @brief       Address-trace back.
 *
 * If you call the @ref trace_print() function a stack traceback of all return
 * addresses up to @ref TRACE_SIZE will be printed from the point of execution.
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef TRACE_H
#define TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of return addresses to print
 */
#ifndef TRACE_SIZE
#define TRACE_SIZE  (4U)
#endif

/**
 * @brief   Print the last @ref TRACE_SIZE return addresses from call of this
 *          function
 */
void trace_print(void);

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */
/** @} */
