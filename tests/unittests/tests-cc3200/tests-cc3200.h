/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Unittests for the ``core`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef TESTS_CORE_H_
#define TESTS_CORE_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_cc3200(void);


/**
 * @brief   Generates tests for lifo.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_core_lifo_tests(void);


#ifdef __cplusplus
}
#endif

#endif /* TESTS_CORE_H_ */
/** @} */
