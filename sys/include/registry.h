/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry RIOT Registry
 * @ingroup     sys
 * @brief       RIOT Registry module for handling runtime configurations
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

#include <stdint.h>
#include <stdbool.h>
#include "clist.h"
#include "xfa.h"
#include "modules.h"
#include "registry/error.h"

/**
 * @brief Identifier of a configuration namespace.
 * It is unique within the scope of the RIOT registry itself.
 */
typedef uint8_t registry_namespace_id_t;

/**
 * @brief Identifier of a configuration schema.
 * It is unique within the scope of its parent configuration namespace.
 */
typedef uint32_t registry_schema_id_t;

/**
 * @brief Identifier of a schema instance.
 * It is unique within the scope of its parent configuration schema.
 */
typedef uint16_t registry_instance_id_t;

/**
 * @brief Identifier of a configuration group or parameter.
 * It is unique within the scope of its parent schema instance.
 * Because a configuration group and a configuration parameter share the same
 * namespace, this type exists for cases where it is not clear if an ID belongs
 * to a group or a parameter.
 */
typedef uint8_t registry_group_or_parameter_id_t;

/**
 * @brief Identifier of a configuration group.
 * It is unique within the scope of its parent schema instance.
 */
typedef registry_group_or_parameter_id_t registry_group_id_t;

/**
 * @brief Identifier of a configuration parameter.
 * It is unique within the scope of its parent schema instance.
 */
typedef registry_group_or_parameter_id_t registry_parameter_id_t;

/**
 * @brief Data structure of a configuration namespace see @p _registry_namespace_t.
 */
typedef struct _registry_namespace_t registry_namespace_t;

/**
 * @brief Data structure of a configuration schema see @p _registry_schema_t.
 */
typedef struct _registry_schema_t registry_schema_t;

/**
 * @brief  Instance of a schema containing its configuration parameters
 * values see @p _registry_instance_t.
 */
typedef struct _registry_instance_t registry_instance_t;

/**
 * @brief  Data structure of a configuration group see @p _registry_group_t.
 */
typedef struct _registry_group_t registry_group_t;

/**
 * @brief Data structure of a configuration parameter see @p _registry_parameter_t.
 */
typedef struct _registry_parameter_t registry_parameter_t;

/**
 * @brief Supported data types used by configuration parameters of the registry.
 */
typedef enum {
    REGISTRY_TYPE_NONE = 0, /**< No type specified */

    REGISTRY_TYPE_OPAQUE, /**< OPAQUE. */
    REGISTRY_TYPE_STRING, /**< String. */
    REGISTRY_TYPE_BOOL,   /**< Boolean. */

    REGISTRY_TYPE_UINT8,  /**< 8-bits unsigned integer. */
    REGISTRY_TYPE_UINT16, /**< 16-bits unsigned integer. */
    REGISTRY_TYPE_UINT32, /**< 32-bits unsigned integer. */
    REGISTRY_TYPE_UINT64, /**< 64-bits unsigned integer. */

    REGISTRY_TYPE_INT8,  /**< 8-bits signed integer. */
    REGISTRY_TYPE_INT16, /**< 16-bits signed integer. */
    REGISTRY_TYPE_INT32, /**< 32-bits signed integer. */
    REGISTRY_TYPE_INT64, /**< 64-bits signed integer. */

    REGISTRY_TYPE_FLOAT32, /**< 32-bits float. */
    REGISTRY_TYPE_FLOAT64, /**< 64-bits float. */
} registry_type_t;

/**
 * @brief The type of a registry node.
 * A registry node points to a namespace, schema, instance, group or parameter.
 */
typedef enum {
    REGISTRY_NODE_NAMESPACE = 0,
    REGISTRY_NODE_SCHEMA,
    REGISTRY_NODE_INSTANCE,
    REGISTRY_NODE_GROUP,
    REGISTRY_NODE_PARAMETER,
} registry_node_type_t;

/**
 * @brief A registry node represents a specific location within the
 * registry configuration tree.
 * It can point to a namespace, schema, instance, group or parameter.
 * The configuration group and the configuration parameter contain a pointer to a
 * schema instance, because they are children of a specific schema instance
 * in the configuration tree.
 */
typedef struct {
    registry_node_type_t type; /**< The type of the node */
    union {
        const registry_namespace_t *namespace; /**< Pointer to the configuration namespace */
        const registry_schema_t *schema;       /**< Pointer to a configuration schema */
        const registry_instance_t *instance;   /**< Pointer to a schema instance */
        struct {
            const registry_instance_t *instance; /**< Pointer to a schema instance */
            const registry_group_t *group;       /**< Pointer to a configuration group */
        } group;                                 /**< A configuration group
                                                      depends on an instance */
        struct {
            const registry_instance_t *instance;   /**< Pointer to a schema instance */
            const registry_parameter_t *parameter; /**< Pointer to a configuration parameter */
        } parameter;                               /**< A configuration parameter
                                                        depends on an instance */
    } value;                                       /**< The location inside of the
                                                        configuration tree */
} registry_node_t;

/**
 * @brief Basic representation of a configuration parameter value.
 */
typedef struct {
    size_t count;         /**< Number of elements (1 for scalar, >1 for array). */
    registry_type_t type; /**< The type of the configuration parameter value. */
    const void *buf;      /**< Pointer to the buffer containing the value
                               of the configuration parameter. */
    size_t buf_len;       /**< Length of the buffer. */
} registry_value_t;

/**
 * @brief The different scopes of a registry @p apply_cb function.
 */
typedef const enum {
    REGISTRY_APPLY_INSTANCE = 0,
    REGISTRY_APPLY_GROUP,
    REGISTRY_APPLY_PARAMETER,
} registry_apply_cb_scope_t;

/**
 * @brief The callback must be implemented by users of a configuration schema.
 * This callback is called when the registry notifies the user,
 * that a configuration parameter value has changed.
 *
 * @param[in] scope Scope of what will be applied (a parameter, a group or the whole instance).
 * @param[in] group_or_parameter_id ID of the group or parameter to apply changes to,
 *                                  applies the whole instance on NULL.
 * @param[in] context Context of the instance.
 *
 * @return 0 on success, non-zero on failure.
 */
typedef registry_error_t (*registry_apply_cb_t)(
    const registry_apply_cb_scope_t scope,
    const registry_group_or_parameter_id_t *group_or_parameter_id,
    const void *context);

/**
 * @brief Instance of a schema containing its configuration parameters values.
 * The instance of a configuration schema is created by users that
 * need to expose runtime configurations.
 * The users also need to implement the @p apply_cb
 * function to get informed when configuration changes.
 */
struct _registry_instance_t {
    clist_node_t node;               /**< Linked list node pointing to the next schema instance. */
    const registry_instance_id_t id; /**< ID of the instance within the scope of it's schema */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char *const name;          /**< String describing the instance. */
#endif                               /* CONFIG_REGISTRY_ENABLE_META_NAME */
    const void *const data;          /**< Generated struct containing all
                                          configuration parameters of the schema. */
    const registry_schema_t *schema; /**< Configuration Schema that the
                                          Registry Instance belongs to. */
    registry_apply_cb_t apply_cb;    /**< Will be called after @ref registry_apply()
                                          was called on this instance. */
    void *context;                   /**< Optional context used by the instance */
};

/**
 * @brief Data structure of a configuration group.
 * A configuration group contains further configuration groups and/or configuration parameters.
 * It has an ID that is unique within the scope of its parent configuration schema.
 */
struct _registry_group_t {
    const registry_group_id_t id; /**< Integer representing the ID of the configuration group. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char *const name; /**< String describing the configuration group. */
#endif                      /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char *const description;                 /**< String describing the configuration group
                                                        with more details. */
#endif                                             /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t *const schema;         /**< Configuration Schema that the
                                                        configuration group belongs to. */
    const registry_group_t **const groups;         /**< Array of pointers to all the configuration
                                                        groups that belong to this group. */
    const size_t groups_len;                       /**< Size of groups array. */
    const registry_parameter_t **const parameters; /**< Array of pointers to all the configuration
                                                        parameters that belong to this group. */
    const size_t parameters_len;                   /**< Size of parameters array. */
};

/**
 * @brief Data structure of a configuration parameter.
 * A configuration parameter mostly holds the type information for a configuration value.
 * It has an ID that is unique within the scope of its parent configuration schema.
 */
struct _registry_parameter_t {
    const registry_parameter_id_t id; /**< Integer representing the ID of
                                           the configuration parameter. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char *const name; /**< String describing the configuration parameter. */
#endif                      /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char *const description;         /**< String describing the configuration
                                                parameter with more details. */
#endif                                     /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t *const schema; /**< Configuration Schema that the
                                                configuration parameter belongs to. */
    const registry_type_t type;            /**< Type of the configuration parameter. */
    const size_t count;                    /**< Number of elements (1 for scalar, >1 for array). */
};

/**
 * @brief Data structure of a configuration schema.
 * A configuration schema contains configuration configuration groups and/or
 * configuration parameters.
 * Besides that it also contains a list of instances that hold the actual
 * configuration parameter values.
 * It has an ID that is unique within the scope of its parent configuration namespace.
 */
struct _registry_schema_t {
    const registry_schema_id_t id; /**< Integer representing the ID of the schema. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char *const name; /**< String describing the schema. */
#endif                      /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char *const description;                 /**< String describing the schema with
                                                        more details. */
#endif                                             /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_namespace_t *const namespace;   /**< Configuration Namespace that the
                                                        Configuration Schema belongs to. */
    clist_node_t instances;                        /**< Linked list of schema instances
                                                        @ref registry_instance_t. */
    const registry_group_t **const groups;         /**< Array of pointers to all the configuration
                                                        groups that belong to this schema. */
    const size_t groups_len;                       /**< Size of groups array. */
    const registry_parameter_t **const parameters; /**< Array of pointers to all the configuration
                                                        parameters that belong to this schema. */
    const size_t parameters_len;                   /**< Size of parameters array. */

    /**
     * @brief Mapping to connect configuration parameter IDs with the address in the storage.
     *
     * @param[in] parameter_id ID of the parameter that contains the value.
     * @param[in] instance Pointer to the instance of the schema, that contains the parameter.
     * @param[out] val Pointer to buffer containing the new value.
     * @param[out] val_len Pointer to length of the buffer to store the current value.
     */
    void (*const mapping)(const registry_parameter_id_t parameter_id,
                          const registry_instance_t *instance,
                          void **val,
                          size_t *val_len);
};

/**
 * @brief Data structure of a configuration namespace.
 * A configuration namespace contains configuration schemas.
 * It exists to prevent ID collisions between RIOT internal configuration schemas
 * and schemas defined by applications running on RIOT.
 * It has an ID that is unique within the scope of the RIOT registry itself.
 */
struct _registry_namespace_t {
    const registry_namespace_id_t id; /**< Integer representing the ID of the namespace. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char *const name; /**< String describing the configuration namespace. */
#endif                      /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char *const description;           /**< String describing the configuration
                                                  namespace with more details. */
#endif                                       /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t **const schemas; /**< Array of pointers to all the configuration
                                                  schemas that belong to this namespace. */
    const size_t schemas_len;                /**< Size of schemas array. */
};

/**
 * @brief Add a namespace to the cross-file-array containing all
 * registry namespaces.
 */
#define REGISTRY_ADD_NAMESPACE(_name, _namespace)                    \
    XFA_USE_CONST(registry_namespace_t *, _registry_namespaces_xfa); \
    XFA_ADD_PTR(_registry_namespaces_xfa, _name, _name, &_namespace)

/**
 * @brief Initializes the RIOT Registry.
 */
void registry_init(void);

/**
 * @brief Adds a new instance to a schema.
 *
 * @param[in] schema Pointer to the schema.
 * @param[in] instance Pointer to the new instance.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_add_schema_instance(
    const registry_schema_t *schema,
    const registry_instance_t *instance);

/**
 * @brief Gets the current value of a parameter that belongs to an instance
 * of a configuration schema.
 *
 * @param[in]  node A location within the registry configuration tree.
 * @param[out] value Pointer to a uninitialized @ref registry_value_t struct.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_get(
    const registry_node_t *node,
    registry_value_t *value);

/**
 * @brief Sets the value of a configuration parameter that belongs
 * to an instance of a configuration schema.
 *
 * @param[in] node A location within the registry configuration tree.
 * @param[in] buf Pointer to the new value for the configuration parameter.
 * @param[in] buf_len Length of the buffer.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_set(
    const registry_node_t *node,
    const void *buf,
    const size_t buf_len);

/**
 * @brief Applies every configuration parameter within the given configuration
 * location (@p node) of the registry configuration tree.
 *
 * @param[in] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_apply(const registry_node_t *node);

/**
 * @brief Callback definition of the @p registry_export function.
 * This callback will be called for each location inside of the
 * configuration tree that is within the scope of the registry node passed
 * on to the @p registry_export function.
 *
 * @param[in] node A location within the registry configuration tree.
 * @param[in] context Context that is passed by the @p registry_export function.
 *
 * @return 0 on success, non-zero on failure.
 */
typedef registry_error_t (*registry_export_cb_t)(
    const registry_node_t *node,
    const void *context);

/**
 * @brief This mode exports every children of the given node including all
 * nested children.
 */
#define REGISTRY_EXPORT_ALL                           0

/**
 * @brief This mode only exports the given node.
 */
#define REGISTRY_EXPORT_SELF                          1

/**
 * @brief This mode exports the given node plus 1 - n levels of children.
 */
#define REGISTRY_EXPORT_WITH_N_LEVELS_OF_CHILDREN(_n) (_n + 1)

/**
 * @brief Exports every configuration parameter within the given configuration
 * location (@p node) of the registry configuration tree.
 *
 * @param[in] node A location within the registry configuration tree.
 * @param[in] export_cb Exporting callback function will be called for each
 * namespace, schema, instance, group and parameter that are in scope.
 * @param[in] tree_traversal_depth Defines how deeply nested child
 * groups / parameters will be shown:
 * 0 to show all children.
 * 1 to only show the exact match.
 * n > 1 to show the exact match plus n - 1 levels of children.
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_export(
    const registry_node_t *node,
    const registry_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context);

#ifdef __cplusplus
}
#endif
/** @} */
