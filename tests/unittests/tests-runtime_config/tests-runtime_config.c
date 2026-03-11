/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Unittest entry point for the runtime config test group
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit/embUnit.h"

#include "tests-runtime_config.h"

Test *tests_runtime_config_get_set_tests(void);
Test *tests_runtime_config_apply_tests(void);
Test *tests_runtime_config_export_tests(void);

void tests_runtime_config(void)
{
    TESTS_RUN(tests_runtime_config_get_set_tests());
    TESTS_RUN(tests_runtime_config_apply_tests());
    TESTS_RUN(tests_runtime_config_export_tests());
}
