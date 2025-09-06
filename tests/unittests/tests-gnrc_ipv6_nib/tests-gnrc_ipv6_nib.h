/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Unittests for the ``gnrc_ipv6_nib`` module
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_gnrc_ipv6_nib(void);

/**
 * @brief   Generates tests for internal layer
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_ipv6_nib_internal_tests(void);

/**
 * @brief   Generates tests for authoritative border router list view
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_ipv6_nib_abr_tests(void);

/**
 * @brief   Generates tests for forwarding table view
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_ipv6_nib_ft_tests(void);

/**
 * @brief   Generates tests for neighbor cache view
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_ipv6_nib_nc_tests(void);

/**
 * @brief   Generates tests for prefix list view
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_gnrc_ipv6_nib_pl_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
