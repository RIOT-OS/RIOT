/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-gtimer.h
 * @brief       Unittests for the ``gtimer`` module
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 */
#ifndef __TESTS_CORE_H_
#define __TESTS_CORE_H_

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

#endif /* __TESTS_CORE_H_ */
/** @} */
