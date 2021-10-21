/*
 * Copyright 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``coding`` module
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef TESTS_CODING_H
#define TESTS_CODING_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_coding(void);

/**
 * @brief   Generates tests for coding/xor.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_coding_xor_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_CODING_H */
/** @} */
