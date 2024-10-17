/*
 * Copyright (C) 2018 Eistec AB
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
 * @brief       Unittests for the ``frac`` header
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef TESTS_FRAC_H
#define TESTS_FRAC_H
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_frac(void);

/**
 * @brief   Generates tests for frac
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_frac_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TESTS_FRAC_H */
