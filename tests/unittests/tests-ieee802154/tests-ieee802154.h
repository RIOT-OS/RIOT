/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``ieee802154`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_ieee802154(void);

/**
 * @brief   Generates tests for ieee802154_timings.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_ieee802154_timings_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
