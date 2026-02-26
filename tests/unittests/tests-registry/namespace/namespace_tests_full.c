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
#include "tests/full.h"

/* Mapping */
static void mapping(const registry_parameter_id_t parameter_id, const registry_instance_t *instance,
                    void **val, size_t *val_len)
{
    registry_tests_full_instance_t *_instance =
        (registry_tests_full_instance_t *)instance->data;

    switch (parameter_id) {
    case REGISTRY_TESTS_FULL_OPAQUE:
        *val = &_instance->opaque;
        *val_len = sizeof(_instance->opaque);
        break;

    case REGISTRY_TESTS_FULL_STRING:
        *val = &_instance->string;
        *val_len = sizeof(_instance->string);
        break;

    case REGISTRY_TESTS_FULL_BOOLEAN:
        *val = &_instance->boolean;
        *val_len = sizeof(_instance->boolean);
        break;

    case REGISTRY_TESTS_FULL_U8:
        *val = &_instance->u8;
        *val_len = sizeof(_instance->u8);
        break;

    case REGISTRY_TESTS_FULL_U16:
        *val = &_instance->u16;
        *val_len = sizeof(_instance->u16);
        break;

    case REGISTRY_TESTS_FULL_U32:
        *val = &_instance->u32;
        *val_len = sizeof(_instance->u32);
        break;

    case REGISTRY_TESTS_FULL_U64:
        *val = &_instance->u64;
        *val_len = sizeof(_instance->u64);
        break;

    case REGISTRY_TESTS_FULL_I8:
        *val = &_instance->i8;
        *val_len = sizeof(_instance->i8);
        break;

    case REGISTRY_TESTS_FULL_I16:
        *val = &_instance->i16;
        *val_len = sizeof(_instance->i16);
        break;

    case REGISTRY_TESTS_FULL_I32:
        *val = &_instance->i32;
        *val_len = sizeof(_instance->i32);
        break;

    case REGISTRY_TESTS_FULL_I64:
        *val = &_instance->i64;
        *val_len = sizeof(_instance->i64);
        break;

    case REGISTRY_TESTS_FULL_F32:
        *val = &_instance->f32;
        *val_len = sizeof(_instance->f32);
        break;

    case REGISTRY_TESTS_FULL_F64:
        *val = &_instance->f64;
        *val_len = sizeof(_instance->f64);
        break;
    }
}

/* Schema parameters */
const registry_parameter_t registry_tests_full_opaque = {
    .id = REGISTRY_TESTS_FULL_OPAQUE,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "opaque",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_OPAQUE,
    .count = 1,
};

const registry_parameter_t registry_tests_full_string = {
    .id = REGISTRY_TESTS_FULL_STRING,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "string",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_STRING,
    .count = 1,
};

const registry_parameter_t registry_tests_full_boolean = {
    .id = REGISTRY_TESTS_FULL_BOOLEAN,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "boolean",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_BOOL,
    .count = 1,
};

const registry_parameter_t registry_tests_full_u8 = {
    .id = REGISTRY_TESTS_FULL_U8,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u8",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

const registry_parameter_t registry_tests_full_u16 = {
    .id = REGISTRY_TESTS_FULL_U16,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u16",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_UINT16,
    .count = 1,
};

const registry_parameter_t registry_tests_full_u32 = {
    .id = REGISTRY_TESTS_FULL_U32,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u32",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_UINT32,
    .count = 1,
};

const registry_parameter_t registry_tests_full_u64 = {
    .id = REGISTRY_TESTS_FULL_U64,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "u64",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_UINT64,
    .count = 1,
};

const registry_parameter_t registry_tests_full_i8 = {
    .id = REGISTRY_TESTS_FULL_I8,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i8",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_INT8,
    .count = 1,
};

const registry_parameter_t registry_tests_full_i16 = {
    .id = REGISTRY_TESTS_FULL_I16,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i16",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_INT16,
    .count = 1,
};

const registry_parameter_t registry_tests_full_i32 = {
    .id = REGISTRY_TESTS_FULL_I32,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i32",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_INT32,
    .count = 1,
};

const registry_parameter_t registry_tests_full_i64 = {
    .id = REGISTRY_TESTS_FULL_I64,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "i64",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_INT64,
    .count = 1,
};

const registry_parameter_t registry_tests_full_f32 = {
    .id = REGISTRY_TESTS_FULL_F32,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "f32",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_FLOAT32,
    .count = 1,
};

const registry_parameter_t registry_tests_full_f64 = {
    .id = REGISTRY_TESTS_FULL_F64,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "f64",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_tests_full,
    .type = REGISTRY_TYPE_FLOAT64,
    .count = 1,
};

/* Schema */
registry_schema_t registry_tests_full = {
    .id = REGISTRY_TESTS_FULL,
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "full",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .namespace = &registry_tests,
    .mapping = mapping,
    .groups = NULL,
    .groups_len = 0,
    .parameters = (const registry_parameter_t *[]){
        &registry_tests_full_opaque,
        &registry_tests_full_string,
        &registry_tests_full_boolean,
        &registry_tests_full_u8,
        &registry_tests_full_u16,
        &registry_tests_full_u32,
        &registry_tests_full_u64,
        &registry_tests_full_i8,
        &registry_tests_full_i16,
        &registry_tests_full_i32,
        &registry_tests_full_i64,
        &registry_tests_full_f32,
        &registry_tests_full_f64,
    },
    .parameters_len = 13,
};
