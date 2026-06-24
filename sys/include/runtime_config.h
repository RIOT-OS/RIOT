/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_runtime_config Runtime configuration
 * @ingroup     sys
 * @brief       Runtime configuration module for handling configurations at runtime
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "clist.h"
#include "xfa.h"
#include "modules.h"
#include "runtime_config/error.h"

#ifndef CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME
/**
 * @brief Enables the "name" field inside the @ref runtime_config_namespace_t,
 *        @ref runtime_config_schema_t, @ref runtime_config_schema_instance_t,
 *        @ref runtime_config_parameter_t, or @ref runtime_config_group_t structs.
 */
#  define CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME 0
#endif

#ifndef CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION
/**
 * @brief Enables the "description" field inside the @ref runtime_config_namespace_t,
 *        @ref runtime_config_schema_t, @ref runtime_config_schema_instance_t,
 *        @ref runtime_config_parameter_t, or @ref runtime_config_group_t structs.
 */
#  define CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION 0
#endif

/**
 * @brief Identifier of a configuration namespace.
 *
 * It is unique within the scope of the runtime configuration system itself.
 */
typedef uint8_t runtime_config_namespace_id_t;

/**
 * @brief Identifier of a configuration schema.
 *
 * It is unique within the scope of its parent configuration namespace.
 */
typedef uint32_t runtime_config_schema_id_t;

/**
 * @brief Identifier of a schema instance.
 *
 * It is unique within the scope of its parent configuration schema.
 */
typedef uint16_t runtime_config_schema_instance_id_t;

/**
 * @brief Identifier of a configuration group or parameter.
 *
 * It is unique within the scope of its parent schema instance.
 * Because a configuration group and a configuration parameter share the same
 * namespace, this type exists for cases where it is not clear if an ID belongs
 * to a group or a parameter.
 */
typedef uint8_t runtime_config_group_or_parameter_id_t;

/**
 * @brief Identifier of a configuration group.
 *
 * It is unique within the scope of its parent schema instance.
 */
typedef runtime_config_group_or_parameter_id_t runtime_config_group_id_t;

/**
 * @brief Identifier of a configuration parameter.
 *
 * It is unique within the scope of its parent schema instance.
 */
typedef runtime_config_group_or_parameter_id_t runtime_config_parameter_id_t;

/**
 * @brief Forward declaration of runtime configuration namespace struct.
 */
typedef struct runtime_config_namespace runtime_config_namespace_t;

/**
 * @brief Forward declaration of runtime configuration schema struct.
 */
typedef struct runtime_config_schema runtime_config_schema_t;

/**
 * @brief Forward declaration of runtime configuration schema instance struct.
 */
typedef struct runtime_config_schema_instance runtime_config_schema_instance_t;

/**
 * @brief Forward declaration of runtime configuration group struct.
 */
typedef struct runtime_config_group runtime_config_group_t;

/**
 * @brief Forward declaration of runtime configuration parameter struct.
 */
typedef struct runtime_config_parameter runtime_config_parameter_t;

/**
 * @brief Supported data types used by configuration parameters of the runtime
 *        config module.
 */
typedef enum {
    RUNTIME_CONFIG_TYPE_BYTES = 1,  /**< Bytes. */
    RUNTIME_CONFIG_TYPE_STRING = 2, /**< String. */
    RUNTIME_CONFIG_TYPE_BOOL = 3,   /**< Boolean. */

    RUNTIME_CONFIG_TYPE_UINT8 = 4,  /**< 8-bits unsigned integer. */
    RUNTIME_CONFIG_TYPE_UINT16 = 5, /**< 16-bits unsigned integer. */
    RUNTIME_CONFIG_TYPE_UINT32 = 6, /**< 32-bits unsigned integer. */
    RUNTIME_CONFIG_TYPE_UINT64 = 7, /**< 64-bits unsigned integer. */

    RUNTIME_CONFIG_TYPE_INT8 = 8,   /**< 8-bits signed integer. */
    RUNTIME_CONFIG_TYPE_INT16 = 9,  /**< 16-bits signed integer. */
    RUNTIME_CONFIG_TYPE_INT32 = 10, /**< 32-bits signed integer. */
    RUNTIME_CONFIG_TYPE_INT64 = 11, /**< 64-bits signed integer. */

    RUNTIME_CONFIG_TYPE_FLOAT32 = 12, /**< 32-bits float. */
    RUNTIME_CONFIG_TYPE_FLOAT64 = 13, /**< 64-bits float. */
} runtime_config_type_t;

/**
 * @brief The type of a runtime configuration node.
 *
 * A runtime configuration node points to a namespace, schema, schema instance,
 * group or parameter.
 */
typedef enum {
    /** The node points to @ref runtime_config_namespace_t */
    RUNTIME_CONFIG_NODE_TYPE_NAMESPACE = 0,

    /** The node points to @ref runtime_config_schema_t */
    RUNTIME_CONFIG_NODE_TYPE_SCHEMA,

    /** The node points to @ref runtime_config_schema_instance_t */
    RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE,

    /** The node points to @ref runtime_config_group_t */
    RUNTIME_CONFIG_NODE_TYPE_GROUP,

    /** The node points to @ref runtime_config_parameter_t */
    RUNTIME_CONFIG_NODE_TYPE_PARAMETER,
} runtime_config_node_type_t;

/**
 * @brief A runtime configuration node represents a specific location within the
 * runtime configuration tree.
 *
 * It can point to a namespace, schema, schema instance, group or parameter.
 * The configuration group and the configuration parameter contain a pointer to
 * a schema instance, because they are children of a specific schema instance
 * in the configuration tree. The schema instance contains the actual values of
 * configuration parameters.
 *
 * The main use-case of this node is to make the @ref runtime_config_get and
 * @ref runtime_config_set functions more ergonomic.
 * Using this struct, other configuration identifiers (such as an array of IDs
 * or names) that point to a specific location in the tree can be converted to a
 * @ref runtime_config_node_t, which can then be passed easily to target functions.
 */
typedef struct {
    /** The type of the node */
    runtime_config_node_type_t type;

    /** The location inside of the configuration tree */
    union {
        /** Pointer to the configuration namespace */
        const runtime_config_namespace_t *as_namespace;

        /** Pointer to a configuration schema */
        const runtime_config_schema_t *as_schema;

        /** Pointer to a schema instance */
        const runtime_config_schema_instance_t *as_schema_instance;

        /** A configuration group depends on an schema instance */
        struct {
            /** Pointer to a schema instance */
            const runtime_config_schema_instance_t *schema_instance;

            /** Pointer to a configuration group */
            const runtime_config_group_t *group;
        } as_group;

        /** A configuration parameter depends on an instance */
        struct {
            /** Pointer to a schema instance */
            const runtime_config_schema_instance_t *schema_instance;

            /** Pointer to a configuration parameter */
            const runtime_config_parameter_t *parameter;
        } as_parameter;
    };
} runtime_config_node_t;

/**
 * @brief Initializes a runtime configuration node pointing to a configuration
 *        namespace.
 *
 * @param[in] NAMESPACE Pointer to the target @ref runtime_config_namespace_t.
 */
#define RUNTIME_CONFIG_NODE_NAMESPACE(NAMESPACE)    \
    {                                               \
        .type = RUNTIME_CONFIG_NODE_TYPE_NAMESPACE, \
        .as_namespace = NAMESPACE,                  \
    }

/**
 * @brief Initializes a runtime configuration node pointing to a configuration
 *        schema.
 *
 * @param[in] SCHEMA Pointer to the target @ref runtime_config_schema_t.
 */
#define RUNTIME_CONFIG_NODE_SCHEMA(SCHEMA)       \
    {                                            \
        .type = RUNTIME_CONFIG_NODE_TYPE_SCHEMA, \
        .as_schema = SCHEMA,                     \
    }

/**
 * @brief Initializes a runtime configuration node pointing to a configuration
 *        schema instance.
 *
 * @param[in] SCHEMA_INSTANCE Pointer to the target @ref runtime_config_schema_instance_t.
 */
#define RUNTIME_CONFIG_NODE_SCHEMA_INSTANCE(SCHEMA_INSTANCE) \
    {                                                        \
        .type = RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE,    \
        .as_schema_instance = SCHEMA_INSTANCE,               \
    }

/**
 * @brief Initializes a runtime configuration node pointing to a configuration group.
 *
 * @param[in] SCHEMA_INSTANCE Pointer to the @ref runtime_config_schema_instance_t
 *                            that this group belongs to.
 * @param[in] GROUP           Pointer to the target @ref runtime_config_group_t.
 */
#define RUNTIME_CONFIG_NODE_GROUP(SCHEMA_INSTANCE, GROUP) \
    {                                                     \
        .type = RUNTIME_CONFIG_NODE_TYPE_GROUP,           \
        .as_group = {                                     \
            .schema_instance = SCHEMA_INSTANCE,           \
            .group = GROUP,                               \
        },                                                \
    }

/**
 * @brief Initializes a runtime configuration node pointing to a configuration parameter.
 *
 * @param[in] SCHEMA_INSTANCE Pointer to the @ref runtime_config_schema_instance_t
 *                            that this parameter belongs to.
 * @param[in] PARAMETER       Pointer to the target @ref runtime_config_parameter_t.
 */
#define RUNTIME_CONFIG_NODE_PARAMETER(SCHEMA_INSTANCE, PARAMETER) \
    {                                                             \
        .type = RUNTIME_CONFIG_NODE_TYPE_PARAMETER,               \
        .as_parameter = {                                         \
            .schema_instance = SCHEMA_INSTANCE,                   \
            .parameter = PARAMETER,                               \
        },                                                        \
    }

/**
 * @brief Callback to apply all parameters within a specific schema instance.
 *
 * This is triggered when a full synchronization is required or when multiple
 * changes occur simultaneously. It allows for atomic updates, such as setting
 * R, G, and B values at once to prevent flickering or invalid intermediate states.
 *
 * @param[in] schema_instance Pointer to the instance containing the full dataset
 *                            and context to be applied.
 *
 * @return 0 on success, non-zero error code on failure.
 */
typedef runtime_config_error_t (*runtime_config_apply_schema_instance_cb_t)(
    const runtime_config_schema_instance_t *const schema_instance);

/**
 * @brief Callback to apply changes to a specific group or parameter.
 *
 * This is triggered for granular updates. Depending on the system design, the
 * @p group_or_parameter_id refers to either a logical collection of parameters
 * or a single leaf node value.
 *
 * @param[in] schema_instance The parent instance context.
 * @param[in] group_or_parameter_id  The identifier for the specific group or
 *                                   parameter that has changed.
 *
 * @return 0 on success, non-zero error code on failure.
 */
typedef runtime_config_error_t (*runtime_config_apply_group_or_parameter_cb_t)(
    const runtime_config_schema_instance_t *const schema_instance,
    const runtime_config_group_or_parameter_id_t group_or_parameter_id);

/**
 * @brief Instance of a schema containing its configuration parameter values.
 *
 * The instance of a configuration schema is created by consumers of that schema
 * who need to expose runtime configurations.
 * The consumers also need to implement the `apply_cb` function to get informed
 * when configurations change.
 */
struct runtime_config_schema_instance {
    /** Linked list node pointing to the next schema instance. */
    clist_node_t node;

    /** ID of the instance within the scope of its schema. */
    runtime_config_schema_instance_id_t id;

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    /** String describing the instance. */
    const char *const name;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration group with more details. */
    const char *const description;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */

    /** Generated struct containing all configuration parameters of the schema.
     * Usually generated from schema YAML files using the "make runtime_config
     * generate" command. */
    const void *const data;

    /** Configuration schema that the instance belongs to. */
    const runtime_config_schema_t *schema;

    /**
     * @brief Called when a full synchronization of the schema instance is
     *        requested.
     *
     * Triggered when @ref runtime_config_apply is called targeting the entire
     * instance.
     * Use this for global updates or atomic multi-parameter synchronization.
     */
    runtime_config_apply_schema_instance_cb_t apply_schema_instance_cb;

    /**
     * @brief Called when a specific group or parameter update is requested.
     *
     * Triggered when @ref runtime_config_apply is called targeting a specific
     * ID. This allows for localized updates without re-processing the entire
     * schema instance.
     */
    runtime_config_apply_group_or_parameter_cb_t apply_group_or_parameter_cb;

    /** Optional context used by the schema instance. */
    void *context;
};

/**
 * @brief Data structure of a configuration group.
 *
 * A configuration group contains further configuration groups and/or
 * configuration parameters. It has an ID that is unique within the scope
 * of its parent configuration schema.
 */
struct runtime_config_group {
    /** Integer representing the ID of the configuration group. */
    const runtime_config_group_id_t id;

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration group. */
    const char *const name;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration group with more details. */
    const char *const description;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */

    /** Configuration schema that the configuration group belongs to. */
    const runtime_config_schema_t *const schema;

    /** Array of pointers to all the configuration groups that belong to this
     * group. */
    const runtime_config_group_t **const groups;

    /** Size of groups array. */
    const size_t groups_len;

    /** Array of pointers to all the configuration parameters that belong to
     * this group. */
    const runtime_config_parameter_t **const parameters;

    /** Size of parameters array. */
    const size_t parameters_len;
};

/**
 * @brief Data structure of a configuration parameter.
 *
 * A configuration parameter mostly holds the type information for a configuration value.
 * It has an ID that is unique within the scope of its parent configuration schema.
 */
struct runtime_config_parameter {
    /** Integer representing the ID of the configuration parameter. */
    const runtime_config_parameter_id_t id;

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration parameter. */
    const char *const name;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration parameter with more details. */
    const char *const description;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */

    /** Configuration schema that the configuration parameter belongs to. */
    const runtime_config_schema_t *const schema;

    /** Type of the configuration parameter. */
    const runtime_config_type_t type;

    /** Number of elements (1 for scalar, >1 for array). */
    const size_t count;
};

/**
 * @brief Data structure of a configuration schema.
 *
 * A configuration schema contains configuration groups and/or
 * configuration parameters.
 * Besides that, it also contains a list of schema instances that hold the
 * actual configuration parameter values.
 * It has an ID that is unique within the scope of its parent configuration namespace.
 */
struct runtime_config_schema {
    /** Integer representing the ID of the schema. */
    const runtime_config_schema_id_t id;

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    /** String describing the schema. */
    const char *const name;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    /** String describing the schema with more details. */
    const char *const description;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */

    /** Configuration namespace that the schema belongs to. */
    const runtime_config_namespace_t *const namespace;

    /** Linked list of schema instances (@ref runtime_config_schema_instance_t). */
    clist_node_t instances;

    /** Array of pointers to all the configuration groups that belong to this schema. */
    const runtime_config_group_t **const groups;

    /** Size of groups array. */
    const size_t groups_len;

    /** Array of pointers to all the configuration parameters that belong to this schema. */
    const runtime_config_parameter_t **const parameters;

    /** Size of parameters array. */
    const size_t parameters_len;

    /**
     * @brief Get the value of a configuration parameter of a specific schema
     *        instance.
     *
     * A configuration schema has many parameters, and a configuration schema
     * instance stores their concrete values in a struct provided in the `data`
     * field. With this function, it is possible to get a pointer to the values
     * of this struct based on the ID of the configuration parameter.
     * This way, it is possible for the runtime_config module to make the
     * connection between the configuration schema meta-data and the actual
     * data in the schema instances.
     *
     * @param[in] parameter_id ID of the parameter that contains the value.
     * @param[in] instance Pointer to the schema instance that contains the parameter.
     * @param[out] val Pointer to buffer containing the new value.
     * @param[out] val_len Pointer to length of the buffer to store the current value.
     *
     * @internal
     */
    void (*const get_parameter_value_from_instance)(
        const runtime_config_parameter_id_t parameter_id,
        const runtime_config_schema_instance_t *instance,
        void **val,
        size_t *val_len);
};

/**
 * @brief Data structure of a configuration namespace.
 *
 * A configuration namespace contains configuration schemas.
 * It exists to prevent ID collisions between RIOT internal configuration
 * schemas and schemas defined by applications running on RIOT.
 * It has an ID that is unique within the scope of the runtime configuration
 * itself.
 */
struct runtime_config_namespace {
    /** Integer representing the ID of the namespace. */
    const runtime_config_namespace_id_t id;

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration namespace. */
    const char *const name;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */

#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    /** String describing the configuration namespace with more details. */
    const char *const description;
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_DESCRIPTION */

    /** Array of pointers to all the configuration schemas that belong to this namespace. */
    const runtime_config_schema_t **const schemas;

    /** Size of schemas array. */
    const size_t schemas_len;
};

/**
 * @brief Add a namespace to the cross-file-array containing all runtime
 *        configuration namespaces.
 */
#define RUNTIME_CONFIG_ADD_NAMESPACE(_name, _namespace)                          \
    XFA_USE_CONST(runtime_config_namespace_t *, _runtime_config_namespaces_xfa); \
    XFA_ADD_PTR(_runtime_config_namespaces_xfa, _name, _name, &_namespace)

/**
 * @brief Initializes the runtime configuration system.
 */
void runtime_config_init(void);

/**
 * @brief Adds a new instance to a schema.
 *
 * @param[in] schema Pointer to the schema.
 * @param[in] schema_instance Pointer to the new instance.
 *
 * @return 0 on success, non-zero on failure.
 */
runtime_config_error_t runtime_config_add_schema_instance(
    runtime_config_schema_t *schema,
    runtime_config_schema_instance_t *schema_instance);

/**
 * @brief Gets a pointer to the current value of a parameter that belongs to an
 *        instance of a configuration schema.
 *
 * @param[in]  node A location within the runtime configuration tree.
 *                  Must be of the type "RUNTIME_CONFIG_NODE_TYPE_PARAMETER".
 * @param[out] buf Pointer to a pointer that will be updated to point to
 *                 the internal configuration value. Must not be NULL.
 * @param[out] buf_len Pointer to a variable where the length of the
 *                     configuration value will be written. Must not be NULL.
 *
 * @return 0 on success, non-zero on failure.
 */
runtime_config_error_t runtime_config_get(
    const runtime_config_node_t *node,
    void **buf,
    size_t *buf_len);

/**
 * @brief Sets the value of a configuration parameter that belongs
 *        to an instance of a configuration schema.
 *
 * @param[in] node A location within the runtime configuration tree.
 *                 Must be of the type "RUNTIME_CONFIG_NODE_TYPE_PARAMETER".
 * @param[in] buf Pointer to the new value for the configuration parameter.
 *                Must not be NULL.
 * @param[in] buf_len Length of the buffer. Must be larger than 0.
 *
 * @return 0 on success, non-zero on failure.
 */
runtime_config_error_t runtime_config_set(
    const runtime_config_node_t *node,
    const void *buf,
    const size_t buf_len);

/**
 * @brief Applies every configuration parameter within the given configuration
 *        location (@ref runtime_config_node_t) of the runtime configuration tree.
 *
 * @param[in] node Optional location within the runtime configuration tree.
 *                 Applies all existing configurations if NULL.
 *
 * @return 0 on success, non-zero on failure.
 */
runtime_config_error_t runtime_config_apply(const runtime_config_node_t *node);

/**
 * @brief Callback definition of the @ref runtime_config_traverse_config_tree
 *        function.
 *
 * This callback will be called for each location inside of the
 * configuration tree that is within the scope of the runtime configuration node
 * passed on to the @ref runtime_config_traverse_config_tree function.
 *
 * @param[in] node A location within the runtime configuration tree.
 * @param[in] context Context that is passed by the
 *                    @ref runtime_config_traverse_config_tree function.
 *
 * @return 0 on success, non-zero on failure.
 */
typedef runtime_config_error_t (*runtime_config_tree_traversal_cb_t)(
    const runtime_config_node_t *node,
    const void *context);

/**
 * @brief In this mode, the function traverses through every child of the
 *        given node including all sub-children when calling the
 *        @ref runtime_config_traverse_config_tree function.
 */
#define RUNTIME_CONFIG_TRAVERSE_ALL                                0

/**
 * @brief In this mode, the function only traverses through the given node when
 *        calling the @ref runtime_config_traverse_config_tree function.
 */
#define RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE                        1

/**
 * @brief This mode traverses through the tree until the given node plus n-1
 *        levels of children when calling the
 *        @ref runtime_config_traverse_config_tree function.
 */
#define RUNTIME_CONFIG_TRAVERSE_TREE_WITH_N_LEVELS_OF_CHILDREN(_n) (((_n) + 1))

/**
 * @brief Iterates over every configuration parameter within the given
 *        configuration location (@ref runtime_config_node_t) of the runtime
 *        configuration tree.
 *
 * @param[in] node A location within the runtime configuration tree.
 * @param[in] tree_traversal_cb Callback function that will be called for
 *                              each namespace, schema, schema instance, group
 *                              and parameter that are within the
 *                              @p tree_traversal_depth and specified @p node.
 * @param[in] tree_traversal_depth Defines how deeply nested child
 *                                 groups / parameters will be shown:
 *                                 0 to show all children,
 *                                 1 to only show the exact match,
 *                                 and n > 1 to show the exact match plus n - 1
 *                                 levels of children.
 * @param[in] context Context that will be passed to @p tree_traversal_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
runtime_config_error_t runtime_config_traverse_config_tree(
    const runtime_config_node_t *node,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context);

#ifdef __cplusplus
}
#endif
/** @} */
