/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "tests-gnrc_ipv6_nib.h"

void tests_gnrc_ipv6_nib(void)
{
    TESTS_RUN(tests_gnrc_ipv6_nib_internal_tests());
    TESTS_RUN(tests_gnrc_ipv6_nib_abr_tests());
    TESTS_RUN(tests_gnrc_ipv6_nib_ft_tests());
    TESTS_RUN(tests_gnrc_ipv6_nib_nc_tests());
    TESTS_RUN(tests_gnrc_ipv6_nib_pl_tests());
}
