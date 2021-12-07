/*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
