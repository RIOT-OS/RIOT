/*
 * Copyright (C) 2016 Martin Landsmann
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
 * @brief       Unittests for the ``tweetnacl`` package
 *
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */
#ifndef TESTS_TWEETNACL_H
#define TESTS_TWEETNACL_H

#include "embUnit/embUnit.h"
#include "random.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MANDATORY function for collecting random Bytes
 *        required by the tweetnacl package
 */
extern void randombytes(uint8_t *target, uint64_t n);

/**
*  @brief   The entry point of this test suite.
*/
void tests_tweetnacl(void);

/**
 * @brief   Generates tests for tweetnacl
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_tweetnacl_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_TWEETNACL_H */
/** @} */
