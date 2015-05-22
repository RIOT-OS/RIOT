/*
 * Copyright (C) 2015 Martin Landsmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``fib`` module
 *
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */
#ifndef TESTS_FIB_H_
#define TESTS_FIB_H_
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_fib(void);

/**
 * @brief   Generates tests for FIB
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_fib_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_FIB_H_ */
/** @} */
