/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_string_path RIOT Registry String Path
 * @ingroup     sys
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

#ifndef REGISTRY_STRING_PATH_H
#define REGISTRY_STRING_PATH_H

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
 * @param[out] path_len Length of the path.
 *
 * @return Length of string path on success, non-zero on failure.
 */
int registry_node_to_string_path(const registry_node_t *node, char *path);

/**
 * @brief Converts a string path to a registry namespace.
 *
 * @param[in]  path The string path array to convert.
 * @param[in]  path_len Length of the path.
 * @param[out] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_node_from_string_path(const char **path, const size_t path_len,
                                                registry_node_t *node);

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_STRING_PATH_H */
/** @} */
