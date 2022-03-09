/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

void tests_ztimer(void)
{
    TESTS_RUN(tests_ztimer_mock_tests());
    TESTS_RUN(tests_ztimer_convert_muldiv64_tests());
}
/** @} */
