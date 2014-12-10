/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-bloom.h
 * @brief       Unittests for the ``bloom`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 */
#ifndef __TESTS_BLOOM_H_
#define __TESTS_BLOOM_H_

#include "../unittests.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_bloom(void);

/**
 * @brief   Generates tests for bloom
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_bloom_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_BLOOM_H_ */
/** @} */
