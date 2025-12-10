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
 * @brief       Unittests for the ``base64`` module
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
void tests_base64(void);

/**
 * @brief   Generates tests for base64
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_base64_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
