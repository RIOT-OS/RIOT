/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
    TESTS_RUN(tests_core_list_tests());
    TESTS_RUN(tests_core_priority_queue_tests());
    TESTS_RUN(tests_core_byteorder_tests());
    TESTS_RUN(tests_core_ringbuffer_tests());
    TESTS_RUN(tests_core_xfa_tests());
}
