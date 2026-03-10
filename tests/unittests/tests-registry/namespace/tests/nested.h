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
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/* NESTED */
extern const registry_parameter_t registry_tests_nested_parameter;
extern const registry_group_t registry_tests_nested_group;
extern const registry_parameter_t registry_tests_nested_group_parameter;
extern registry_schema_t registry_tests_nested;

typedef struct {
    clist_node_t node;
    uint8_t parameter;
    uint8_t group_parameter;
} registry_tests_nested_instance_t;

typedef enum {
    REGISTRY_TESTS_NESTED_PARAMETER,
    REGISTRY_TESTS_NESTED_GROUP,
    REGISTRY_TESTS_NESTED_GROUP_PARAMETER,
} registry_tests_nested_indices_t;

#ifdef __cplusplus
}
#endif
/** @} */
