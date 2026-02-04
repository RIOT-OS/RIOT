/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-core.h"

void tests_core(void)
{
    TESTS_RUN(tests_core_atomic_tests());
    TESTS_RUN(tests_core_bitarithm_tests());
    TESTS_RUN(tests_core_cib_tests());
    TESTS_RUN(tests_core_clist_tests());
    TESTS_RUN(tests_core_list_tests());
    TESTS_RUN(tests_core_mbox_tests());
    TESTS_RUN(tests_core_priority_queue_tests());
    TESTS_RUN(tests_core_byteorder_tests());
    TESTS_RUN(tests_core_ringbuffer_tests());
    TESTS_RUN(tests_core_xfa_tests());
    TESTS_RUN(tests_core_macros_tests());
}
