/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry RIOT Registry
 * @ingroup     module_registry
 * @brief       RIOT Registry for runtime configuration of modules
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "clist.h"

#include "registry.h"
#include "registry/util.h"

/* Implementation of the module */

XFA_INIT_CONST(registry_namespace_t *, _registry_namespaces_xfa);

int registry_add_schema_instance(const registry_schema_t *schema,
                                 const registry_instance_t *instance)
{
    assert(schema != NULL);
    assert(instance != NULL);

    /* add schema to instance */
    (*((registry_instance_t *)instance)).schema = schema;

    /* get instances length to determine the id of the new instance */
    size_t count = clist_count((clist_node_t *)&schema->instances);

    /* set id of new instance to the instance count */
    *(registry_instance_id_t *)&instance->id = count;

    /* add instance to schema */
    clist_rpush((clist_node_t *)&schema->instances, (clist_node_t *)&instance->node);

    return 0;
}

int registry_get(const registry_instance_t *instance, const registry_parameter_t *parameter,
                 registry_value_t *value)
{
    assert(instance != NULL);
    assert(parameter != NULL);
    assert(value != NULL);

    /* call handler to get pointer to registry internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    parameter->schema->mapping(parameter->id, instance, &intern_val, &intern_val_len);

    /* update buf pointer in registry_value_t to point to the value inside the registry and set buf_len */
    value->type = parameter->type;
    value->buf = intern_val;
    value->buf_len = intern_val_len;

    return 0;
}

int registry_set(const registry_instance_t *instance, const registry_parameter_t *parameter,
                 const void *buf, const size_t buf_len)
{
    assert(instance != NULL);
    assert(parameter != NULL);
    assert(buf != NULL);

    /* get pointer to registry internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    parameter->schema->mapping(parameter->id, instance, &intern_val, &intern_val_len);

    if (buf_len > intern_val_len) {
        return -EINVAL;
    }

    /* call handler to apply the new value to the correct parameter in the instance of the schema */
    memcpy(intern_val, buf, buf_len);

    return 0;
}

int registry_commit(void)
{
    int rc = 0;

    /* commit all namespaces */
    for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
        registry_namespace_t *namespace = _registry_namespaces_xfa[i];

        int _rc = registry_commit_namespace(namespace);

        if (!_rc) {
            rc = _rc;
        }
    }

    return rc;
}

int registry_commit_namespace(const registry_namespace_t *namespace)
{
    assert(namespace != NULL);

    int rc = 0;

    /* commit all configuration schemas of the given namespace if available */
    for (size_t i = 0; i < namespace->schemas_len; i++) {
        const registry_schema_t *child = namespace->schemas[i];

        int _rc = registry_commit_schema(child);

        if (!_rc) {
            rc = _rc;
        }
    }

    return rc;
}

int registry_commit_schema(const registry_schema_t *schema)
{
    assert(schema != NULL);

    int rc = 0;

    /* commit all configuration schema instances of the given configuration schema if available */
    clist_node_t *node = schema->instances.next;

    if (!node) {
        return -EINVAL;
    }

    do {
        node = node->next;
        registry_instance_t *instance = container_of(node, registry_instance_t, node);

        if (!instance) {
            return -EINVAL;
        }

        int _rc = registry_commit_instance(instance);

        if (!_rc) {
            rc = _rc;
        }
    } while (node != schema->instances.next);

    return rc;
}

int registry_commit_instance(const registry_instance_t *instance)
{
    return instance->commit_cb(REGISTRY_COMMIT_INSTANCE, NULL, instance->context);
}

int registry_commit_group(const registry_instance_t *instance, const registry_group_t *group)
{
    return instance->commit_cb(REGISTRY_COMMIT_GROUP, &group->id, instance->context);
}

int registry_commit_parameter(const registry_instance_t *instance,
                              const registry_parameter_t *parameter)
{
    return instance->commit_cb(REGISTRY_COMMIT_PARAMETER, &parameter->id, instance->context);
}

int registry_export(const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                    const void *context)
{
    int rc = 0;

    /* export all namespaces */
    for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
        registry_namespace_t *namespace = _registry_namespaces_xfa[i];

        int _rc = registry_export_namespace(namespace, export_cb, recursion_depth, context);

        if (!_rc) {
            rc = _rc;
        }
    }

    return rc;
}

int registry_export_namespace(const registry_namespace_t *namespace,
                              const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                              const void *context)
{
    assert(namespace != NULL);

    /* export the given namespace */
    registry_export_cb_data_t export_data = { .namespace = namespace };
    int rc = export_cb(&export_data, REGISTRY_EXPORT_NAMESPACE, context);

    /* export all configuration schemas of the given namespace if available and within recursion_depth bounds */
    if (recursion_depth == 1) {
        return 0;
    }
    else {
        int new_recursion_depth = recursion_depth;
        if (recursion_depth > 1) {
            new_recursion_depth--;
        }

        for (size_t i = 0; i < namespace->schemas_len; i++) {
            const registry_schema_t *child = namespace->schemas[i];

            int _rc = registry_export_schema(child, export_cb, new_recursion_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

int registry_export_schema(const registry_schema_t *schema, const registry_export_cb_t export_cb,
                           const uint8_t recursion_depth, const void *context)
{
    assert(schema != NULL);

    /* export the given configuration schema */
    registry_export_cb_data_t export_data = { .schema = schema };
    int rc = export_cb(&export_data, REGISTRY_EXPORT_SCHEMA, context);

    /* export all instances of the given configuration schema if available and within recursion_depth bounds */
    if (recursion_depth == 1) {
        return 0;
    }
    else {
        int new_recursion_depth = recursion_depth;
        if (recursion_depth > 1) {
            new_recursion_depth--;
        }

        clist_node_t *node = schema->instances.next;

        if (!node) {
            return -EINVAL;
        }

        do {
            node = node->next;
            registry_instance_t *instance = container_of(node, registry_instance_t, node);

            if (!instance) {
                return -EINVAL;
            }

            int _rc = registry_export_instance(instance, export_cb, new_recursion_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        } while (node != schema->instances.next);
    }

    return rc;
}

int registry_export_instance(const registry_instance_t *instance,
                             const registry_export_cb_t export_cb, const uint8_t recursion_depth,
                             const void *context)
{
    assert(instance != NULL);

    /* export the given configuration schema instance */
    registry_export_cb_data_t export_data = { .instance = instance };
    int rc = export_cb(&export_data, REGISTRY_EXPORT_INSTANCE, context);

    /* export all groups or parameters of the given configuration schema instance if available and within recursion_depth bounds */
    if (recursion_depth == 1) {
        return 0;
    }
    else {
        int new_recursion_depth = recursion_depth;
        if (recursion_depth > 1) {
            new_recursion_depth--;
        }

        int _rc = rc;

        /* groups */
        for (size_t i = 0; i < instance->schema->groups_len; i++) {
            _rc = registry_export_group(instance, instance->schema->groups[i], export_cb,
                                        new_recursion_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        }

        /* parameters */
        for (size_t i = 0; i < instance->schema->parameters_len; i++) {
            _rc = registry_export_parameter(instance, instance->schema->parameters[i], export_cb,
                                            context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

int registry_export_group(const registry_instance_t *instance, const registry_group_t *group,
                          const registry_export_cb_t export_cb,
                          const uint8_t recursion_depth, const void *context)
{
    assert(group != NULL);

    /* export the given configuration group */
    registry_export_cb_data_t export_data = { .group = group };
    int rc = export_cb(&export_data, REGISTRY_EXPORT_GROUP, context);

    /* export all children of the given configuration group if available and within recursion_depth bounds */
    if (recursion_depth == 1) {
        return 0;
    }
    else {
        int new_recursion_depth = recursion_depth;
        if (recursion_depth > 1) {
            new_recursion_depth--;
        }

        int _rc = rc;

        /* group */
        for (size_t i = 0; i < group->groups_len; i++) {
            _rc = registry_export_group(instance, group->groups[i], export_cb, new_recursion_depth,
                                        context);

            if (!_rc) {
                rc = _rc;
            }
        }

        /* parameter */
        for (size_t i = 0; i < group->parameters_len; i++) {
            _rc = registry_export_parameter(instance, group->parameters[i], export_cb, context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

int registry_export_parameter(const registry_instance_t *instance,
                              const registry_parameter_t *parameter,
                              const registry_export_cb_t export_cb, const void *context)
{
    assert(parameter != NULL);

    registry_export_cb_data_t export_data = {
        .parameter = {
            .data = parameter,
            .instance = instance,
        }
    };
    return export_cb(&export_data, REGISTRY_EXPORT_PARAMETER, context);
}
