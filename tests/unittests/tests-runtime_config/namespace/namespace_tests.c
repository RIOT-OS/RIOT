/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @brief       Runtime config "tests" namespace providing configuration
 *              schemas for testint the runtime_config sys module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "kernel_defines.h"
#include "runtime_config.h"

#include "tests.h"
#include "tests/full.h"
#include "tests/nested.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const runtime_config_schema_t *_schemas[] = {
    &runtime_config_tests_full,
    &runtime_config_tests_nested,
};

runtime_config_namespace_t runtime_config_tests = {
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "tests",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "Tests namespace",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schemas = _schemas,
    .schemas_len = ARRAY_SIZE(_schemas),
};

RUNTIME_CONFIG_ADD_NAMESPACE(tests, runtime_config_tests);
