/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittest entry point for the Registry test group
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit/embUnit.h"

#include "tests-registry.h"

Test *tests_registry_get_set_tests(void);
Test *tests_registry_apply_tests(void);
Test *tests_registry_export_tests(void);

void tests_registry(void)
{
    TESTS_RUN(tests_registry_get_set_tests());
    TESTS_RUN(tests_registry_apply_tests());
    TESTS_RUN(tests_registry_export_tests());
}
