/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
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
 * @brief       Unittests for the ``libc`` module
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef TESTS_LIBC_H
#define TESTS_LIBC_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_libc(void);

/**
 * @brief   Generates tests for libc
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_libc_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TESTS_LIBC_H */
