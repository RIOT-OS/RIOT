/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``div`` header
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

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

/** @} */
