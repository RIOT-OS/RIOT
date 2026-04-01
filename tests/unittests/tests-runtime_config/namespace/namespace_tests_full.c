/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @brief       Runtime configuration "full" schema using all possible data
 *              types of the "Runtime configuration" module
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

#include "kernel_defines.h"
#include "runtime_config.h"
#include "tests.h"
#include "tests/full.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Mapping */
static void _get_parameter_value_from_instance(
    const runtime_config_parameter_id_t parameter_id,
    const runtime_config_schema_instance_t *instance,
    void **val,
    size_t *val_len)
{
    runtime_config_tests_full_instance_t *_instance =
        (runtime_config_tests_full_instance_t *)instance->data;

    switch (parameter_id) {
    case RUNTIME_CONFIG_TESTS_FULL_BYTES:
        *val = &_instance->bytes;
        *val_len = sizeof(_instance->bytes);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_STRING:
        *val = &_instance->string;
        *val_len = sizeof(_instance->string);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_BOOLEAN:
        *val = &_instance->boolean;
        *val_len = sizeof(_instance->boolean);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_U8:
        *val = &_instance->u8;
        *val_len = sizeof(_instance->u8);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_U16:
        *val = &_instance->u16;
        *val_len = sizeof(_instance->u16);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_U32:
        *val = &_instance->u32;
        *val_len = sizeof(_instance->u32);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_U64:
        *val = &_instance->u64;
        *val_len = sizeof(_instance->u64);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_I8:
        *val = &_instance->i8;
        *val_len = sizeof(_instance->i8);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_I16:
        *val = &_instance->i16;
        *val_len = sizeof(_instance->i16);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_I32:
        *val = &_instance->i32;
        *val_len = sizeof(_instance->i32);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_I64:
        *val = &_instance->i64;
        *val_len = sizeof(_instance->i64);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_F32:
        *val = &_instance->f32;
        *val_len = sizeof(_instance->f32);
        break;

    case RUNTIME_CONFIG_TESTS_FULL_F64:
        *val = &_instance->f64;
        *val_len = sizeof(_instance->f64);
        break;
    }
}

/* Schema parameters */
const runtime_config_parameter_t runtime_config_tests_full_bytes = {
    .id = RUNTIME_CONFIG_TESTS_FULL_BYTES,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "bytes",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_BYTES,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_string = {
    .id = RUNTIME_CONFIG_TESTS_FULL_STRING,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "string",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_STRING,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_boolean = {
    .id = RUNTIME_CONFIG_TESTS_FULL_BOOLEAN,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "boolean",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_BOOL,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_u8 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_U8,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u8",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_UINT8,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_u16 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_U16,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u16",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_UINT16,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_u32 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_U32,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u32",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_UINT32,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_u64 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_U64,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u64",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_UINT64,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_i8 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_I8,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i8",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_INT8,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_i16 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_I16,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i16",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_INT16,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_i32 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_I32,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i32",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_INT32,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_i64 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_I64,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i64",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_INT64,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_f32 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_F32,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "f32",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_FLOAT32,
    .count = 1,
};

const runtime_config_parameter_t runtime_config_tests_full_f64 = {
    .id = RUNTIME_CONFIG_TESTS_FULL_F64,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "f64",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .schema = &runtime_config_tests_full,
    .type = RUNTIME_CONFIG_TYPE_FLOAT64,
    .count = 1,
};

/* Schema */
runtime_config_schema_t runtime_config_tests_full = {
    .id = RUNTIME_CONFIG_TESTS_FULL,
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "full",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */
    .namespace = &runtime_config_tests,
    .get_parameter_value_from_instance = _get_parameter_value_from_instance,
    .groups = NULL,
    .groups_len = 0,
    .parameters = (const runtime_config_parameter_t *[]){
        &runtime_config_tests_full_bytes,
        &runtime_config_tests_full_string,
        &runtime_config_tests_full_boolean,
        &runtime_config_tests_full_u8,
        &runtime_config_tests_full_u16,
        &runtime_config_tests_full_u32,
        &runtime_config_tests_full_u64,
        &runtime_config_tests_full_i8,
        &runtime_config_tests_full_i16,
        &runtime_config_tests_full_i32,
        &runtime_config_tests_full_i64,
        &runtime_config_tests_full_f32,
        &runtime_config_tests_full_f64,
    },
    .parameters_len = 13,
};
