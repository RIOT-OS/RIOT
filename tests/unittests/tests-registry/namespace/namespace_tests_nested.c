/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_tests_nested RIOT Registry Schema: Nested
 * @ingroup     sys
 * @brief       RIOT Registry Nested Schema representing different nesting levels of a configuration schema
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "registry.h"
#include "tests.h"
#include "tests/nested.h"

/* Mapping */
static void mapping(const registry_parameter_id_t parameter_id, const registry_instance_t *instance,
                    void **val, size_t *val_len)
{
    registry_tests_nested_instance_t *_instance =
        (registry_tests_nested_instance_t *)instance->data;

    switch (parameter_id) {
    case REGISTRY_TESTS_NESTED_PARAMETER:
        *val = &_instance->parameter;
        *val_len = sizeof(_instance->parameter);
        break;

    case REGISTRY_TESTS_NESTED_GROUP_PARAMETER:
        *val = &_instance->group_parameter;
        *val_len = sizeof(_instance->group_parameter);
        break;
    }
}

/* Schema parameters */
const registry_parameter_t registry_tests_nested_parameter = {
    .id = REGISTRY_TESTS_NESTED_PARAMETER,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "parameter",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_nested,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

const registry_parameter_t registry_tests_nested_group_parameter = {
    .id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "parameter",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_nested,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

/* Schema groups */
const registry_group_t registry_tests_nested_group = {
    .id = REGISTRY_TESTS_NESTED_GROUP,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "group",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_nested,
    .groups = NULL,
    .groups_len = 0,
    .parameters = (const registry_parameter_t *[]){
        &registry_tests_nested_group_parameter,
    },
    .parameters_len = 1,
};

/* Schema */
registry_schema_t registry_tests_nested = {
    .id = REGISTRY_TESTS_NESTED,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "nested",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .namespace = &registry_tests,
    .mapping = mapping,
    .groups = (const registry_group_t *[]){
        &registry_tests_nested_group,
    },
    .groups_len = 1,
    .parameters = (const registry_parameter_t *[]){
        &registry_tests_nested_parameter,
    },
    .parameters_len = 1,
};
