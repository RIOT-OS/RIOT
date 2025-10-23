/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_tests RIOT Registry Tests Namespace
 * @ingroup     tests
 * @brief       RIOT Registry Namespace Tests module
 * @{
 *
 * This module provides common test schemas for the RIOT Registry sys module
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

extern registry_namespace_t registry_tests;

/**
 * @brief This ENUM defines the IDs of configuration schemas in the "tests" namespace.
 * The IDs are needed by the int_path module to identify schemas using IDs instead
 * of pointers.
 */
typedef enum {
    REGISTRY_TESTS_FULL,
    REGISTRY_TESTS_NESTED,
} registry_tests_indices_t;

#ifdef __cplusplus
}
#endif

/** @} */
