/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_find RIOT Registry utility to find registry nodes
 * @ingroup     sys
 * @brief       RIOT Registry Find module for finding specific nodes within the configuration tree
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

#include "registry.h"
#include "registry/error.h"

/**
 * @brief The different return types of @p registry_find_comparator_t callback function.
 */
typedef enum {
    REGISTRY_FIND_NO_MATCH = -1,
    REGISTRY_FIND_EXACT_MATCH = 0,
    REGISTRY_FIND_PARTIAL_MATCH = 1,
} registry_find_result_type;

/**
 * @brief This callback is called by the @p registry_find function passing in a @p context,
 * to compare a @p node with it.
 *
 * @param[in] node A location within the registry configuration tree.
 * @param[in] context Context of the callback (contains the data to compare each @p node to).
 *
 * @return 0 on exact match, 1 on partial match and -1 on no match.
 */
typedef registry_find_result_type (*registry_find_comparator_t)(
    const registry_node_t *node,
    const void *context);

/**
 * @brief Finds a specific node within the registry configuration tree.
 * The comparison of nodes is handled by the @p compare callback function.
 * The context to compare to is passed to it using the @p context parameter.
 *
 * @param[in] compare Callback function that is invoked on the nodes of the configuration tree.
 * @param[in] context Optional context used by the @p compare callback function.
 * @param[out] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_find(
    const registry_find_comparator_t compare,
    const void *context,
    registry_node_t *node);

#ifdef __cplusplus
}
#endif
/** @} */
