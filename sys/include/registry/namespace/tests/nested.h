/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef REGISTRY_NAMESPACE_TESTS_NESTED_H
#define REGISTRY_NAMESPACE_TESTS_NESTED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/* NESTED */
#if IS_USED(MODULE_REGISTRY_NAMESPACE_TESTS_NESTED) || IS_ACTIVE(DOXYGEN)

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

#endif

#ifdef __cplusplus
}
#endif

#endif /* MODULE_REGISTRY_NAMESPACE_TESTS_NESTED */
/** @} */
