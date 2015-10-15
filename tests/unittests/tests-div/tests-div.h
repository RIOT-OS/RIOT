/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Unittests for the ``div`` header
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef TESTS_DIV_H_
#define TESTS_DIV_H_
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_div(void);

/**
 * @brief   Generates tests for div
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_div_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_DIV_H_ */
/** @} */
