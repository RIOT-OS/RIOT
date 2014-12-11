/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-core.h
 * @brief       Unittests for the ``core`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __TESTS_CORE_H_
#define __TESTS_CORE_H_

#include "../unittests.h"

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
 * @brief   Generates tests for lifo.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_lifo_tests(void);

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

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CORE_H_ */
/** @} */
