/*
 * Copyright (C) 2015 Kushal Singh <kushal.spiderman.singh@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-hash_string.h
 * @brief       Unittests for the ``hash_string`` module
 *
 * @author      Kushal Singh <kushal.spiderman.singh@gmail.com>
 */
#ifndef TESTS_HASH_STRING_H_
#define TESTS_HASH_STRING_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_hash_string(void);

/**
 * @brief   Generates tests for hash_string
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hash_string_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_HASH_STRING_H_ */
/** @} */
