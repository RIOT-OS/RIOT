/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-core.h"

void tests_core(void)
{
    TESTS_RUN(tests_core_atomic_tests());
    TESTS_RUN(tests_core_bitarithm_tests());
    TESTS_RUN(tests_core_cib_tests());
    TESTS_RUN(tests_core_clist_tests());
    TESTS_RUN(tests_core_lifo_tests());
    TESTS_RUN(tests_core_priority_queue_tests());
}
