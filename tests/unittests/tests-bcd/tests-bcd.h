/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief   Unittests for the `bcd` header
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_bcd(void);

/**
 * @brief   Generates tests for bcd
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_bcd_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
