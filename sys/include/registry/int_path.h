/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_int_path RIOT Registry integer path
 * @ingroup     sys
 * @brief       RIOT Registry integer path module
 * @{
 *
 * This module provides functions to convert between @ref registry_node_t and
 * @ref registry_int_path_t.
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
 * @brief Maximum length of a configuration path.
 *
 * Path: namespace_id / schema_id / instance_id / (group_id | parameter_id).
 */
#define REGISTRY_INT_PATH_MAX_LEN        4

/**
 * @brief Maximum length of a configuration path as a string of numbers.
 *
 * An int path consists of the following elements:
 * namespace_id:             8 bits corresponds to a max of  3 characters
 * schema_id:               32 bits corresponds to a max of 10 characters
 * instance_id:             16 bits corresponds to a max of  5 characters
 * group__or_parameter_id:   8 bits corresponds to a max of  3 characters
 * We also need to include the separator. One additional char between each number.
 */
#define REGISTRY_INT_PATH_STRING_MAX_LEN (3 + 10 + 5 + 3 + (REGISTRY_INT_PATH_MAX_LEN - 1))

/**
 * @brief Integer path representation for a namespace.
 */
typedef struct {
    registry_namespace_id_t namespace_id; /**< The ID of the namespace. */
} registry_namespace_int_path_t;

/**
 * @brief Integer path representation for a configuration schema.
 */
typedef struct {
    registry_namespace_id_t namespace_id; /**< The ID of the namespace. */
    registry_schema_id_t schema_id;       /**< The ID of the schema. */
} registry_schema_int_path_t;

/**
 * @brief Integer path representation for a configuration schema instance.
 */
typedef struct {
    registry_namespace_id_t namespace_id; /**< The ID of the namespace. */
    registry_schema_id_t schema_id;       /**< The ID of the schema. */
    registry_instance_id_t instance_id;   /**< The ID of the instance. */
} registry_instance_int_path_t;

/**
 * @brief Integer path representation for a configuration group.
 */
typedef struct {
    registry_namespace_id_t namespace_id; /**< The ID of the namespace. */
    registry_schema_id_t schema_id;       /**< The ID of the schema. */
    registry_instance_id_t instance_id;   /**< The ID of the instance. */
    registry_group_id_t group_id;         /**< The ID of the group. */
} registry_group_int_path_t;

/**
 * @brief Integer path representation for a configuration parameter.
 */
typedef struct {
    registry_namespace_id_t namespace_id; /**< The ID of the namespace. */
    registry_schema_id_t schema_id;       /**< The ID of the schema. */
    registry_instance_id_t instance_id;   /**< The ID of the instance. */
    registry_parameter_id_t parameter_id; /**< The ID of the parameter. */
} registry_parameter_int_path_t;

/**
 * @brief Integer path representation for a configuration group or parameter.
 */
typedef struct {
    registry_namespace_id_t namespace_id;                   /**< The ID of the namespace. */
    registry_schema_id_t schema_id;                         /**< The ID of the schema. */
    registry_instance_id_t instance_id;                     /**< The ID of the instance. */
    registry_group_or_parameter_id_t group_or_parameter_id; /**< The ID of the group or parameter. */
} registry_group_or_parameter_int_path_t;

/**
 * @brief Enumeration of the different types of integer paths.
 * It mostly just specifies what the leaf of the path is.
 */
typedef enum {
    REGISTRY_INT_PATH_TYPE_NAMESPACE,          /**< The path represents a namespace. */
    REGISTRY_INT_PATH_TYPE_SCHEMA,             /**< The path represents a schema. */
    REGISTRY_INT_PATH_TYPE_INSTANCE,           /**< The path represents an instance. */
    REGISTRY_INT_PATH_TYPE_GROUP,              /**< The path represents a group. */
    REGISTRY_INT_PATH_TYPE_PARAMETER,          /**< The path represents a parameter. */
    REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER, /**< The path represents a group or parameter. */
} registry_int_path_type_t;

/**
 * @brief Union of the different types of integer paths.
 */
typedef struct {
    registry_int_path_type_t type; /**< The type of the integer path. */
    union {
        registry_namespace_int_path_t namespace_path;                   /**< The path represents a namespace. */
        registry_schema_int_path_t schema_path;                         /**< The path represents a schema. */
        registry_instance_int_path_t instance_path;                     /**< The path represents an instance. */
        registry_group_int_path_t group_path;                           /**< The path represents a group. */
        registry_parameter_int_path_t parameter_path;                   /**< The path represents a parameter. */
        registry_group_or_parameter_int_path_t group_or_parameter_path; /**< The path represents a group or parameter. */
    } value;                                                            /**< The concrete integer path depending on its type. */
} registry_int_path_t;

/**
 * @brief Converts a registry namespace to an integer path.
 *
 * @param[in] node A location within the registry configuration tree.
 * @param[out] path The integer path.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_node_to_int_path(
    const registry_node_t *node,
    registry_int_path_t *path);

/**
 * @brief Converts an integer path to a registry namespace.
 *
 * @param[in] path The integer path to convert.
 * @param[out] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_node_from_int_path(
    const registry_int_path_t *path,
    registry_node_t *node);

#ifdef __cplusplus
}
#endif
/** @} */
