/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    test_utils_interactive_sync Test interactive synchronization
 * @ingroup     sys
 * @brief       Utility function for synchronizing before a test
 *
 * @{
 * @file
 * @brief       Synchronization for normally non interactive tests
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wait for the tester to start test
 *
 * @details Wait for a 's' character to return
 *
 */
#ifdef MODULE_TEST_UTILS_INTERACTIVE_SYNC
void test_utils_interactive_sync(void);
#else
static inline void test_utils_interactive_sync(void) {}
#endif

#ifdef __cplusplus
}
#endif
/** @} */
