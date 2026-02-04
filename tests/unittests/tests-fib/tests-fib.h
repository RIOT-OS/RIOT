/*
 * SPDX-FileCopyrightText: 2015 Martin Landsmann
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``fib`` module
 *
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */

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

/** @} */
