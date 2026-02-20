/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_tests RIOT Registry Tests Namespace
 * @ingroup     tests
 * @brief       RIOT Registry Namespace Tests module providing common tests configuration schemas for the RIOT Registry sys module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "registry.h"

#include "tests.h"
#include "tests/full.h"
#include "tests/nested.h"

static const registry_schema_t *_schemas[] = {
    &registry_tests_full,
    &registry_tests_nested,
};

registry_namespace_t registry_tests = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "tests",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "Tests namespace",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schemas = _schemas,
    .schemas_len = ARRAY_SIZE(_schemas),
};

REGISTRY_ADD_NAMESPACE(tests, registry_tests);
