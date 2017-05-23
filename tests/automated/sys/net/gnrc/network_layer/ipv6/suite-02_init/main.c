/*
 * Copyright (C) 2015 Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests-ipv6
 * @{
 *
 * @file
 * @brief       Test application for gnrc_ipv6 network
 *
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/gnrc/nettest.h"
#include "embUnit.h"

#include "../tests-ipv6.h"

int main(void)
{

    /* Preparing minimal runtime environment for modules using netapi. */
    if (gnrc_nettest_init() < 0) {
        puts("Error initializing nettest thread");
        return 1;
    }


    TESTS_START();
    TESTS_RUN(ipv6_tests());
    TESTS_END();

    return 0;
}
