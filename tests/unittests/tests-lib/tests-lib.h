/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-lib.h
 * @brief       Unittests for the ``lib`` sysmodule
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */
#ifndef __TESTS_CORE_H_
#define __TESTS_CORE_H_

#include "../unittests.h"

/**
 * @brief   The entry point of this test suite.
 */
void tests_lib(void);

/**
 * @brief   Generates tests ringbuffer.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_lib_ringbuffer_tests(void);

#endif /* __TESTS_CORE_H_ */
/** @} */
