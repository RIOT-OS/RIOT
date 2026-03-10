/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_tests_full RIOT Registry Schema: Full
 * @ingroup     sys
 * @brief       RIOT Registry Full Schema using all possible data types of the riot registry
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

/* FULL */
extern const registry_parameter_t registry_tests_full_opaque;
extern const registry_parameter_t registry_tests_full_string;
extern const registry_parameter_t registry_tests_full_boolean;
extern const registry_parameter_t registry_tests_full_u8;
extern const registry_parameter_t registry_tests_full_u16;
extern const registry_parameter_t registry_tests_full_u32;
extern const registry_parameter_t registry_tests_full_u64;
extern const registry_parameter_t registry_tests_full_i8;
extern const registry_parameter_t registry_tests_full_i16;
extern const registry_parameter_t registry_tests_full_i32;
extern const registry_parameter_t registry_tests_full_i64;
extern const registry_parameter_t registry_tests_full_f32;
extern const registry_parameter_t registry_tests_full_f64;
extern registry_schema_t registry_tests_full;

typedef struct {
    uint8_t value;
} registry_tests_full_instance_opaque_t;

typedef struct {
    clist_node_t node;
    registry_tests_full_instance_opaque_t opaque;
    char string[50];
    bool boolean;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;
} registry_tests_full_instance_t;

typedef enum {
    REGISTRY_TESTS_FULL_OPAQUE,
    REGISTRY_TESTS_FULL_STRING,
    REGISTRY_TESTS_FULL_BOOLEAN,
    REGISTRY_TESTS_FULL_U8,
    REGISTRY_TESTS_FULL_U16,
    REGISTRY_TESTS_FULL_U32,
    REGISTRY_TESTS_FULL_U64,
    REGISTRY_TESTS_FULL_I8,
    REGISTRY_TESTS_FULL_I16,
    REGISTRY_TESTS_FULL_I32,
    REGISTRY_TESTS_FULL_I64,
    REGISTRY_TESTS_FULL_F32,
    REGISTRY_TESTS_FULL_F64,
} registry_tests_full_indices_t;

#ifdef __cplusplus
}
#endif
/** @} */
