/*
 * Copyright (C) 2015 Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  tests-ipv6 Testing IPV6 module
 * @{
 *
 * @file
 * @brief       Unittests for the `gnrc_ipv6` module
 *
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 */
#ifndef TESTS_IPV6_H_
#define TESTS_IPV6_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Collects all test cases of this test suite.
 */
Test* ipv6_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_IPV6_H_ */
/** @} */
