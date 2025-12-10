/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``core`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_core(void);

/**
 * @brief   Generates tests for atomic.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_atomic_tests(void);

/**
 * @brief   Generates tests for bitarithm.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_bitarithm_tests(void);

/**
 * @brief   Generates tests for cib.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_cib_tests(void);

/**
 * @brief   Generates tests for clist.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_clist_tests(void);

/**
 * @brief   Generates tests for list.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_list_tests(void);

/**
 * @brief   Generates tests for mbox.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_mbox_tests(void);

/**
 * @brief   Generates tests for priority_queue.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_priority_queue_tests(void);

/**
 * @brief   Generates tests for byteorder.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_byteorder_tests(void);

/**
 * @brief   Generates tests for ringbuffer.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_ringbuffer_tests(void);

/**
 * @brief   Generates tests for xfa.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_xfa_tests(void);

/**
 * @brief   Generates tests for macros
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_macros_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
