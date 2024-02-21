/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef REGISTRY_H
#define REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "clist.h"
#include "xfa.h"
#include "modules.h"

typedef uint8_t registry_namespace_id_t;
typedef uint32_t registry_schema_id_t;
typedef uint16_t registry_instance_id_t;
typedef uint16_t registry_group_or_parameter_id_t;
typedef registry_group_or_parameter_id_t registry_group_id_t;
typedef registry_group_or_parameter_id_t registry_parameter_id_t;

typedef struct _registry_namespace_t registry_namespace_t;
typedef struct _registry_schema_t registry_schema_t;
typedef struct _registry_instance_t registry_instance_t;
typedef struct _registry_group_t registry_group_t;
typedef struct _registry_parameter_t registry_parameter_t;

/**
 * @brief Data types of the registry.
 */
typedef enum {
    REGISTRY_TYPE_NONE = 0,         /**< No type specified */

    REGISTRY_TYPE_OPAQUE,           /**< OPAQUE. */
    REGISTRY_TYPE_STRING,           /**< String. */
    REGISTRY_TYPE_BOOL,             /**< Boolean. */

    REGISTRY_TYPE_UINT8,            /**< 8-bits unsigned integer. */
    REGISTRY_TYPE_UINT16,           /**< 16-bits unsigned integer. */
    REGISTRY_TYPE_UINT32,           /**< 32-bits unsigned integer. */
    REGISTRY_TYPE_UINT64,           /**< 64-bits unsigned integer. */

    REGISTRY_TYPE_INT8,             /**< 8-bits signed integer. */
    REGISTRY_TYPE_INT16,            /**< 16-bits signed integer. */
    REGISTRY_TYPE_INT32,            /**< 32-bits signed integer. */
    REGISTRY_TYPE_INT64,            /**< 64-bits signed integer. */

    REGISTRY_TYPE_FLOAT32,          /**< 32-bits float. */
    REGISTRY_TYPE_FLOAT64,          /**< 64-bits float. */
} registry_type_t;

/**
 * @brief Basic representation of a configuration parameter value.
 */
typedef struct {
    registry_type_t type;   /**< The type of the configuration parameter value. */
    const void *buf;        /**< Pointer to the buffer containing the value of the configuration parameter. */
    size_t buf_len;         /**< Length of the buffer. */
} registry_value_t;

typedef const enum {
    REGISTRY_COMMIT_INSTANCE,
    REGISTRY_COMMIT_GROUP,
    REGISTRY_COMMIT_PARAMETER,
} registry_commit_cb_scope_t;

/**
 * @brief Callback must be implemented by modules / drivers that integrate a configuration schema.
 * This callback is called when the registry notifies the module / driver, that a configuration
 * parameter has changed.
 *
 * @param[in] scope Scope of what will be committed (a parameter, a group or the whole instance).
 * @param[in] id ID of the group or parameter to commit changes to, commits the whole instance on NULL.
 * @param[in] context Context of the instance.
 *
 * @return 0 on success, non-zero on failure.
 */
typedef int (*registry_commit_cb_t)(const registry_commit_cb_scope_t scope,
                                    const registry_group_or_parameter_id_t *group_or_parameter_id,
                                    const void *context);

/**
 * @brief Instance of a schema containing its data.
 */
struct _registry_instance_t {
    clist_node_t node;                  /**< Linked list node. */
    const registry_instance_id_t id;    /**< Integer representing the path id of the schema instance. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char * const name;            /**< String describing the instance. */
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    const void * const data;            /**< Struct containing all configuration parameters of the schema. */
    const registry_schema_t *schema;    /**< Configuration Schema that the Schema Instance belongs to. */
    registry_commit_cb_t commit_cb;     /**< Will be called after @ref registry_commit() was called on this instance. */

    void *context;                      /**< Optional context used by the instance */
};

struct _registry_group_t {
    const registry_group_id_t id;                   /**< Integer representing the ID of the configuration group. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char * const name;                        /**< String describing the configuration group. */
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char * const description;                 /**< String describing the configuration group with more details. */
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t * const schema;         /**< Configuration Schema that the configuration group belongs to. */
    const registry_group_t ** const groups;         /**< Array of pointers to all the configuration groups that belong to this group. */
    const size_t groups_len;                        /**< Size of groups array. */
    const registry_parameter_t ** const parameters; /**< Array of pointers to all the configuration parameters that belong to this group. */
    const size_t parameters_len;                    /**< Size of parameters array. */
};

struct _registry_parameter_t {
    const registry_parameter_id_t id;                       /**< Integer representing the ID of the configuration parameter. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char * const name;                                /**< String describing the configuration parameter. */
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char * const description;                         /**< String describing the configuration parameter with more details. */
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t * const schema;                 /**< Configuration Schema that the configuration parameter belongs to. */
    const registry_type_t type;                             /**< Type of the configuration parameter. */
};

/**
 * @brief Schema containing available configuration parameters.
 */
struct _registry_schema_t {
    const registry_schema_id_t id;                          /**< Integer representing the ID of the schema. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char * const name;                                /**< String describing the schema. */
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char * const description;                         /**< String describing the schema with more details. */
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_namespace_t * const namespace;           /**< Configuration Namespace that the Configuration Schema belongs to. */
    clist_node_t instances;                                 /**< Linked list of schema instances @ref registry_instance_t. */
    const registry_group_t ** const groups;                 /**< Array of pointers to all the configuration groups that belong to this schema. */
    const size_t groups_len;                                /**< Size of groups array. */
    const registry_parameter_t ** const parameters;         /**< Array of pointers to all the configuration parameters that belong to this schema. */
    const size_t parameters_len;                            /**< Size of parameters array. */

    /**
     * @brief Mapping to connect configuration parameter IDs with the address in the storage.
     *
     * @param[in] parameter_id ID of the parameter that contains the value.
     * @param[in] instance Pointer to the instance of the schema, that contains the parameter.
     * @param[in] val Pointer to buffer containing the new value.
     * @param[in] val_len Pointer to length of the buffer to store the current value.
     */
    void(*const mapping)(const registry_parameter_id_t parameter_id,
                         const registry_instance_t *instance,
                         void **val,
                         size_t *val_len);
};

struct _registry_namespace_t {
    const registry_namespace_id_t id;           /**< Integer representing the ID of the namespace. */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    const char * const name;                    /**< String describing the configuration namespace. */
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    const char * const description;             /**< String describing the configuration namespace with more details. */
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    const registry_schema_t ** const schemas;   /**< Array of pointers to all the configuration schemas that belong to this namespace. */
    const size_t schemas_len;                   /**< Size of schemas array. */
};

#define REGISTRY_ADD_NAMESPACE(_name, _namespace) \
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
int registry_add_schema_instance(const registry_schema_t *schema,
                                 const registry_instance_t *instance);

/**
 * @brief Gets the current value of a parameter that belongs to an instance
 * of a configuration schema.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 * @param[in] parameter Pointer to the configuration parameter.
 * @param[out] value Pointer to a uninitialized @ref registry_value_t struct.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_get(const registry_instance_t *instance, const registry_parameter_t *parameter,
                 registry_value_t *value);

/**
 * @brief Sets the value of a configuration parameter that belongs to an instance
 * of a configuration schema.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 * @param[in] parameter Pointer to the configuration parameter.
 * @param[in] buf Pointer to the new value for the configuration parameter.
 * @param[in] buf_len Length of the buffer.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_set(const registry_instance_t *instance, const registry_parameter_t *parameter,
                 const void *buf, const size_t buf_len);

/**
 * @brief Commits every configuration parameter.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit(void);

/**
 * @brief Commits every configuration parameter within the given configuration namespace.
 *
 * @param[in] namespace Pointer to the configuration namespace.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit_namespace(const registry_namespace_t *namespace);

/**
 * @brief Commits every configuration parameter within the given configuration schema.
 *
 * @param[in] schema Pointer to the configuration schema.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit_schema(const registry_schema_t *schema);

/**
 * @brief Commits every configuration parameter within the given configuration schema instance.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit_instance(const registry_instance_t *instance);

/**
 * @brief Commits every configuration parameter within the given configuration group.
 *
 * @param[in] instance Pointer to the configuration schema instance of the configuration group.
 * @param[in] group Pointer to the configuration group.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit_group(const registry_instance_t *instance, const registry_group_t *group);

/**
 * @brief Commits the given configuration parameter.
 *
 * @param[in] instance Pointer to the configuration schema instance of the configuration parameter.
 * @param[in] parameter Pointer to the configuration parameter.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_commit_parameter(const registry_instance_t *instance,
                              const registry_parameter_t *parameter);

typedef const union {
    const registry_namespace_t *namespace;
    const registry_schema_t *schema;
    const registry_instance_t *instance;
    const registry_group_t *group;
    const struct {
        const registry_parameter_t *data;
        const registry_instance_t *instance;
    } parameter;
} registry_export_cb_data_t;

typedef const enum {
    REGISTRY_EXPORT_NAMESPACE,
    REGISTRY_EXPORT_SCHEMA,
    REGISTRY_EXPORT_INSTANCE,
    REGISTRY_EXPORT_GROUP,
    REGISTRY_EXPORT_PARAMETER,
} registry_export_cb_data_type_t;

typedef int (*registry_export_cb_t)(const registry_export_cb_data_t *data,
                                    const registry_export_cb_data_type_t data_type,
                                    const void *context);

#define REGISTRY_EXPORT_ALL = 0;
#define REGISTRY_EXPORT_SELF = 1;
#define REGISTRY_EXPORT_WITH_N_LEVELS_OF_CHILDREN(_n) (_n + 1);

/**
 * @brief Exports every configuration parameter.
 *
 * @param[in] export_cb Exporting callback function will be called for each namespace,
 * schema, instance, group and parameter that are in scope.
 * @param[in] recursion_depth Defines how deeply nested child groups / parameters
 * will be shown. (0 to show all children, 1 to only show the exact match, n > 1
 * to show the exact match plus n - 1 levels of children).
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export(const registry_export_cb_t export_cb,
                    const uint8_t recursion_depth, const void *context);

/**
 * @brief Exports every configuration parameter within the given configuration namespace.
 *
 * @param[in] namespace Pointer to the configuration namespace.
 * @param[in] export_cb Exporting callback function will be called for the given namespace
 * and each of its schemas, instances, groups and parameters that are in scope.
 * @param[in] recursion_depth Defines how deeply nested child groups / parameters
 * will be shown. (0 to show all children, 1 to only show the exact match, 2 - n
 * to show the exact match plus its children ... plus n levels of children).
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export_namespace(const registry_namespace_t *namespace,
                              const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                              const void *context);

/**
 * @brief Exports every configuration parameter within the given configuration schema.
 *
 * @param[in] schema Pointer to the configuration schema.
 * @param[in] export_cb Exporting callback function will be called for the given schema
 * and each of its instances, groups and parameters that are in scope.
 * @param[in] recursion_depth Defines how deeply nested child groups / parameters
 * will be shown. (0 to show all children, 1 to only show the exact match, 2 - n
 * to show the exact match plus its children ... plus n levels of children).
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export_schema(const registry_schema_t *schema, const registry_export_cb_t export_cb,
                           const uint8_t recursion_depth, const void *context);

/**
 * @brief Exports every configuration parameter within the given configuration schema instance.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 * @param[in] export_cb Exporting callback function will be called for the given instance
 * and each of its groups and parameters that are in scope.
 * @param[in] recursion_depth Defines how deeply nested child groups / parameters
 * will be shown. (0 to show all children, 1 to only show the exact match, 2 - n
 * to show the exact match plus its children ... plus n levels of children).
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export_instance(const registry_instance_t *instance,
                             const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                             const void *context);

/**
 * @brief Exports every configuration parameter within the given configuration group.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 * @param[in] group Pointer to the configuration group.
 * @param[in] export_cb Exporting callback function will be called for the given group
 * and each of its subgroups and parameters that are in scope.
 * @param[in] recursion_depth Defines how deeply nested child groups / parameters
 * will be shown. (0 to show all children, 1 to only show the exact match, 2 - n
 * to show the exact match plus its children ... plus n levels of children).
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export_group(const registry_instance_t *instance, const registry_group_t *group,
                          const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                          const void *context);

/**
 * @brief Exports the given configuration parameter.
 *
 * @param[in] instance Pointer to the configuration schema instance.
 * @param[in] parameter Pointer to the configuration parameter.
 * @param[in] export_cb Exporting callback function will be called for the given parameter.
 * @param[in] context Context that will be passed to @p export_cb.
 *
 * @return 0 on success, non-zero on failure.
 */
int registry_export_parameter(const registry_instance_t *instance,
                              const registry_parameter_t *parameter,
                              const registry_export_cb_t export_cb, const void *context);



#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_H */
/** @} */
