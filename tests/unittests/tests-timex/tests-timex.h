/*
 * Copyright (C) 2014 Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-timex.h
 * @brief       Unittests for the ``timex`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author      Daniel Jentsch <d.jentsch@fu-berlin.de>
 */
#ifndef __TESTS_TIMEX_H_
#define __TESTS_TIMEX_H_

#include "../unittests.h"

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

#endif /* __TESTS_TIMEX_H_ */
/** @} */
