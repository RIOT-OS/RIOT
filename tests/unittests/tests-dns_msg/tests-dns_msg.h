/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the `dns_msg` module
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef TESTS_DNS_MSG_H
#define TESTS_DNS_MSG_H

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
#endif /* TESTS_DNS_MSG_H */
