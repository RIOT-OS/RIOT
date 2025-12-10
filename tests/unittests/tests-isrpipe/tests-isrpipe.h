/*
 * SPDX-FileCopyrightText: 2025 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for ISR pipe
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Entry point of the test suite
 */
void tests_isrpipe(void);

/**
 * @brief Generates tests for isrpipe.h
 *
 * @return embUnit tests if successful, NULL if not.
 */
Test *tests_isrpipe_tests(void);

/**
 * @brief Generates tests for isrpipe/read_timeout.h
 *
 * @return embUnit tests if successful, NULL if not.
 */
Test *tests_isrpipe_read_timeout_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
