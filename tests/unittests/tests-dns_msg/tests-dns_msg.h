/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the `dns_msg` module
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_dns_msg(void);

/**
 * @brief   Generates tests for dns_msg
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_dns_msg_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
