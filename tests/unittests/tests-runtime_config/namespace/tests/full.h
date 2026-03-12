/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @brief       Runtime config Full Schema using all possible data types of the
 *              Runtime config
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

/* FULL */
extern const runtime_config_parameter_t runtime_config_tests_full_opaque;
extern const runtime_config_parameter_t runtime_config_tests_full_string;
extern const runtime_config_parameter_t runtime_config_tests_full_boolean;
extern const runtime_config_parameter_t runtime_config_tests_full_u8;
extern const runtime_config_parameter_t runtime_config_tests_full_u16;
extern const runtime_config_parameter_t runtime_config_tests_full_u32;
extern const runtime_config_parameter_t runtime_config_tests_full_u64;
extern const runtime_config_parameter_t runtime_config_tests_full_i8;
extern const runtime_config_parameter_t runtime_config_tests_full_i16;
extern const runtime_config_parameter_t runtime_config_tests_full_i32;
extern const runtime_config_parameter_t runtime_config_tests_full_i64;
extern const runtime_config_parameter_t runtime_config_tests_full_f32;
extern const runtime_config_parameter_t runtime_config_tests_full_f64;
extern runtime_config_schema_t runtime_config_tests_full;

typedef struct {
    clist_node_t node;
    void opaque[10];
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
} runtime_config_tests_full_instance_t;

typedef enum {
    RUNTIME_CONFIG_TESTS_FULL_OPAQUE,
    RUNTIME_CONFIG_TESTS_FULL_STRING,
    RUNTIME_CONFIG_TESTS_FULL_BOOLEAN,
    RUNTIME_CONFIG_TESTS_FULL_U8,
    RUNTIME_CONFIG_TESTS_FULL_U16,
    RUNTIME_CONFIG_TESTS_FULL_U32,
    RUNTIME_CONFIG_TESTS_FULL_U64,
    RUNTIME_CONFIG_TESTS_FULL_I8,
    RUNTIME_CONFIG_TESTS_FULL_I16,
    RUNTIME_CONFIG_TESTS_FULL_I32,
    RUNTIME_CONFIG_TESTS_FULL_I64,
    RUNTIME_CONFIG_TESTS_FULL_F32,
    RUNTIME_CONFIG_TESTS_FULL_F64,
} runtime_config_tests_full_indices_t;

#ifdef __cplusplus
}
#endif
/** @} */
