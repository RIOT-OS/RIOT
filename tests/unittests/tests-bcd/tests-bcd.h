/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief   Unittests for the `bcd` header
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef TESTS_BCD_H
#define TESTS_BCD_H

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_bcd(void);

/**
 * @brief   Generates tests for bcd
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_bcd_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_BCD_H */
/** @} */
