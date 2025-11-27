/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``frac`` header
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_frac(void);

/**
 * @brief   Generates tests for frac
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_frac_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
