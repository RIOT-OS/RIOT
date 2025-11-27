/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``credman`` module
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief   The entry point of this test suite.
 */
void tests_credman(void);

/**
 * @brief   Generates tests for credman
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_credman_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
