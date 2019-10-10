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
 * @brief       Unittests for the ``core_tasklet`` module
 *
 * @author      Jose I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef TESTS_TASKLET_H
#define TESTS_TASKLET_H
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_tasklet(void);

/**
 * @brief   Generates tests for tasklet
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_tasklet_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_TASKLET_H */
/** @} */
