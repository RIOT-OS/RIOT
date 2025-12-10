/*
 * SPDX-FileCopyrightText: 2014 Philipp Rosenkranz
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``bloom`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 */

#include "embUnit.h"

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

/** @} */
