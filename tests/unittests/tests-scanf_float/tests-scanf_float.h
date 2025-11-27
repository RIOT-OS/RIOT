/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Test scanf with floating-point numbers (scanf_float).
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generates tests for scanf_float
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_scanf_float_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
