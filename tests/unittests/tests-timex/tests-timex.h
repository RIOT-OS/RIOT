/*
 * Copyright (C) 2014 Philipp Rosenkranz, Daniel Jentsch
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
 * @brief       Unittests for the ``timex`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author      Daniel Jentsch <d.jentsch@fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_timex(void);

/**
 * @brief   Generates tests for timex
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_timex_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
