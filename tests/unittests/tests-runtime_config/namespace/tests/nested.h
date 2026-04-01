/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @brief       Runtime configuration "nested" schema representing different
 *              nesting levels of a configuration schema
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "runtime_config.h"

/* NESTED */
extern const runtime_config_parameter_t runtime_config_tests_nested_parameter;
extern const runtime_config_group_t runtime_config_tests_nested_group;
extern const runtime_config_parameter_t runtime_config_tests_nested_group_parameter;
extern runtime_config_schema_t runtime_config_tests_nested;

typedef struct {
    clist_node_t node;
    uint8_t parameter;
    uint8_t group_parameter;
} runtime_config_tests_nested_instance_t;

typedef enum {
    RUNTIME_CONFIG_TESTS_NESTED_PARAMETER,
    RUNTIME_CONFIG_TESTS_NESTED_GROUP,
    RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER,
} runtime_config_tests_nested_indices_t;

#ifdef __cplusplus
}
#endif
/** @} */
