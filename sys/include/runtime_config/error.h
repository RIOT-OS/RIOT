/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_runtime_config_error Runtime configuration error
 * @ingroup     sys_runtime_config
 * @brief       Runtime configuration "Error" module providing runtime
 *              configuration module specific error codes
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runtime configuration module specific error codes.
 */
typedef enum {
    RUNTIME_CONFIG_ERROR_NONE = 0,
    RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT,
    RUNTIME_CONFIG_ERROR_NAMESPACE_NOT_FOUND,
    RUNTIME_CONFIG_ERROR_SCHEMA_NOT_FOUND,
    RUNTIME_CONFIG_ERROR_INSTANCE_NOT_FOUND,
    RUNTIME_CONFIG_ERROR_GROUP_NOT_FOUND,
    RUNTIME_CONFIG_ERROR_PARAMETER_NOT_FOUND,
    RUNTIME_CONFIG_ERROR_NODE_INVALID,
    RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_SMALL,
    RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_LARGE,
} runtime_config_error_t;

#ifdef __cplusplus
}
#endif

/** @} */
