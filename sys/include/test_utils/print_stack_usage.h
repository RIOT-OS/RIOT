/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    test_utils_print_stack_usage Print stack usage
 * @ingroup     sys
 * @brief       Print the stack usage of threads in JSON metric format
 *
 * When the module `test_utils_print_stack_usage` is used, the stack usage of
 * each thread is printed when it exits, and the stack usage of the idle
 * thread is printed when `main()` returns.
 *
 * @{
 * @file
 * @brief       Print the stack usage of threads
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print the stack usage of a stack in JSON metric format
 *
 * @param[in]   name        Name of the thread to print.
 * @param[in]   stack       Start of the stack, as passed to thread_create().
 * @param[in]   max_size    Size of the stack in bytes, as passed to
 *                          thread_create().
 */
void print_stack_usage_metric(const char *name, void *stack, unsigned max_size);

#ifdef __cplusplus
}
#endif
/** @} */
