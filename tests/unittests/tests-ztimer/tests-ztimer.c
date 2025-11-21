/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittest entry point for the ztimer test group
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "embUnit/embUnit.h"

#include "tests-ztimer.h"

Test *tests_ztimer_mock_tests(void);
Test *tests_ztimer_convert_muldiv64_tests(void);
Test *tests_ztimer_ondemand_tests(void);

void tests_ztimer(void)
{
    TESTS_RUN(tests_ztimer_mock_tests());
    TESTS_RUN(tests_ztimer_convert_muldiv64_tests());
    TESTS_RUN(tests_ztimer_ondemand_tests());
}
/** @} */
