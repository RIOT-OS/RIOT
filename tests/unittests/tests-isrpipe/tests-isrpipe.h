/*
 * Copyright (c) 2025 Prime Controls, Inc.(R)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
