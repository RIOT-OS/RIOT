/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef REGISTRY_NAMESPACE_TESTS_H
#define REGISTRY_NAMESPACE_TESTS_H

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

#endif /* REGISTRY_NAMESPACE_TESTS_H */
/** @} */
