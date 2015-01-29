/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-fs.h
 * @brief       Unittests for the ``fs`` module
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 */
#ifndef __TESTS_FS_H_
#define __TESTS_FS_H_
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief   The entry point of this test suite.
*/
void tests_fs(void);

/**
 * @brief   Generates tests for base64
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_fs_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_FS_H_ */
/** @} */
