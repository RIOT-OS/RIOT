/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_runtime_config_namespace_tests Runtime config Tests Namespace
 * @ingroup     tests
 * @brief       Runtime config Namespace Tests module
 * @{
 *
 * This module provides common test schemas for the Runtime config sys module
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "runtime_config.h"

extern runtime_config_namespace_t runtime_config_tests;

/**
 * @brief This ENUM defines the IDs of configuration schemas in the "tests" namespace.
 * The IDs are needed by the int_path module to identify schemas using IDs instead
 * of pointers.
 */
typedef enum {
    RUNTIME_CONFIG_TESTS_FULL,
    RUNTIME_CONFIG_TESTS_NESTED,
} runtime_config_tests_indices_t;

#ifdef __cplusplus
}
#endif

/** @} */
