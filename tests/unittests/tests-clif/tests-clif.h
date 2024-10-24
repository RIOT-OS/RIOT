/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Unittests for the ``clif`` module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#ifndef TESTS_CLIF_H
#define TESTS_CLIF_H
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_clif(void);

/**
 * @brief   Generates tests for link format
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_clif_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TESTS_CLIF_H */
