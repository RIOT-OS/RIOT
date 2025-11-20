/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief   Unittests for the `bit` header
 *
 *          This verifies that the functions to test/set/clear individual bits
 *          (which may use hardware specific instructions to do so) work.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_bit(void);

/**
 * @brief   Generates tests for bit header
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_bit_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
