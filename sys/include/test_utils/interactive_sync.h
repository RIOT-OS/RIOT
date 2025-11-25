/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef TEST_UTILS_INTERACTIVE_SYNC_H
#define TEST_UTILS_INTERACTIVE_SYNC_H

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
#endif /* TEST_UTILS_INTERACTIVE_SYNC_H */
/** @} */
