/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
