/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Unittests for the qDSA package
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef TESTS_QDSA_H
#define TESTS_QDSA_H

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_qDSA(void);

/**
 * @brief   Generates tests for qDSA
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_dDSA_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_QDSA_H */
/** @} */
