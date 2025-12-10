/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``libc`` module
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
void tests_libc(void);

/**
 * @brief   Generates tests for libc
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_libc_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
