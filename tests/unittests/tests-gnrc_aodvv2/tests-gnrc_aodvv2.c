/*
 * Copyright (C) 2016 Lotte Steenbrink <lotte@zombietetris.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-gnrc_aodvv2.h"

void tests_gnrc_aodvv2(void)
{
    TESTS_RUN(tests_gnrc_aodvv2_seqnum_tests());
    TESTS_RUN(tests_gnrc_aodvv2_routingtable_tests());
}

/** @} */