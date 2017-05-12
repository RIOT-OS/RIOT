/*
 * Copyright (C) 2016 Lotte Steenbrink
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-gnrc_aodvv2.h
 * @brief       Unittests for the ``aodvv2`` module
 *
 * @author      Lotte Steenbrink <lotte@zombietetris.de>
 */
#ifndef TESTS_GNRC_AODVV2_H_
#define TESTS_GNRC_AODVV2_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_gnrc_aodvv2(void);

/**
 * @brief   Generates tests for aodvv2/seqnum.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_aodvv2_seqnum_tests(void);

/**
 * @brief   Generates tests for aodvv2/routingtable.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_aodvv2_routingtable_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_GNRC_AODVV2_H_ */
/** @} */
