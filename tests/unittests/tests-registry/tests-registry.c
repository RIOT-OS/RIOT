/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       RIOT Registry module unit tests
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit.h"

Test *tests_registry_set_get_value_tests(void);
Test *tests_registry_load_tests(void);
Test *tests_registry_conversions_tests(void);
Test *tests_registry_export_tests(void);
Test *tests_registry_commit_tests(void);
Test *tests_registry_save_tests(void);

void tests_registry(void)
{
    TESTS_RUN(tests_registry_set_get_value_tests());
    TESTS_RUN(tests_registry_load_tests());
    TESTS_RUN(tests_registry_conversions_tests());
    TESTS_RUN(tests_registry_export_tests());
    TESTS_RUN(tests_registry_commit_tests());
    TESTS_RUN(tests_registry_save_tests());
}
