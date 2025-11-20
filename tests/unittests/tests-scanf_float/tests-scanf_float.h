/*
 * Copyright (C) 2019 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
