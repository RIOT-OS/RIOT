/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_registry_string_path RIOT Registry String Path
 * @ingroup     sys_registry
 * @brief       RIOT Registry String Path module
 * @{
 *
 * This module provides functions to convert between @ref registry_node_t and
 * a string path representation of it.
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"
#include "registry/error.h"

/**
 * @brief Converts a registry namespace to its string path representation.
 *
 * @param[in]  node A location within the registry configuration tree.
 * @param[out] path The buffer to store the resulting string path.
 *
 * @return Length of string path on success, non-zero on failure.
 */
int registry_node_to_string_path(
    const registry_node_t *node,
    char *path);

/**
 * @brief Converts a string path to a registry namespace.
 *
 * @param[in]  path The string path array to convert.
 * @param[in]  path_len Length of the path.
 * @param[out] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_node_from_string_path(
    const char **path,
    const size_t path_len,
    registry_node_t *node);

#ifdef __cplusplus
}
#endif
/** @} */
