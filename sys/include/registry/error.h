/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_error RIOT Registry Error
 * @ingroup     sys
 * @brief       RIOT Registry module providing all possible registry specific error codes
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

/**
 * @brief Registry specific error codes.
 */
typedef enum {
    REGISTRY_ERROR_NONE = 0,
    REGISTRY_ERROR_INVALID_ARGUMENT,
    REGISTRY_ERROR_NAMESPACE_NOT_FOUND,
    REGISTRY_ERROR_SCHEMA_NOT_FOUND,
    REGISTRY_ERROR_INSTANCE_NOT_FOUND,
    REGISTRY_ERROR_GROUP_NOT_FOUND,
    REGISTRY_ERROR_PARAMETER_NOT_FOUND,
    REGISTRY_ERROR_NODE_INVALID,
    REGISTRY_ERROR_BUF_LEN_TOO_SMALL,
    REGISTRY_ERROR_BUF_LEN_TOO_LARGE,
    REGISTRY_ERROR_SCHEMA_HAS_NO_INSTANCE,
} registry_error_t;

#ifdef __cplusplus
}
#endif

/** @} */
