/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``luid`` module
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_luid(void);

/**
 * @brief   Generates tests for luid
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_luid_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
