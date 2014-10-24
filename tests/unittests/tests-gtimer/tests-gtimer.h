/*
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-gtimer.h
 * @brief       Unittests for the ``gtimer`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author      Daniel Jentsch <d.jentsch@fu-berlin.de>
 */
#ifndef __TESTS_GTIMER_H_
#define __TESTS_GTIMER_H_

#define GTIMER_TEST 1
#include "gtimer.h"
#include "../unittests.h"

/**
 * @brief   The entry point of this test suite.
 */
void tests_gtimer(void);

/**
 * @brief   Generates tests for gtimer
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gtimer_tests(void);

#endif /* __TESTS_GTIMER_H_ */
/** @} */
