/*
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittest entry point for the ztimer64 test group
 *
 */

#include "embUnit/embUnit.h"

#include "tests-ztimer64.h"

Test *tests_ztimer64_tests(void);

void tests_ztimer64(void)
{
    TESTS_RUN(tests_ztimer64_tests());
}
/** @} */
