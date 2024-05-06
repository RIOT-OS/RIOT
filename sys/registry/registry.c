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
#include "registry/error.h"

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

int registry_get(const registry_node_t *node, registry_value_t *value)
{
    assert(node != NULL);
    assert(value != NULL);

    if (node->type != REGISTRY_NODE_PARAMETER) {
        return -REGISTRY_ERROR_NODE_INVALID;
    }

    /* call handler to get pointer to registry internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const registry_parameter_t *parameter = node->value.parameter.parameter;

    parameter->schema->mapping(parameter->id, node->value.parameter.instance, &intern_val, &intern_val_len);

    /* update buf pointer in registry_value_t to point to the value inside the registry and set buf_len */
    value->type = parameter->type;
    value->buf = intern_val;
    value->buf_len = intern_val_len;

    return 0;
}

int registry_set(const registry_node_t *node, const void *buf, const size_t buf_len)
{
    assert(node != NULL);
    assert(buf != NULL);

    if (node->type != REGISTRY_NODE_PARAMETER) {
        return -REGISTRY_ERROR_NODE_INVALID;
    }

    /* get pointer to registry internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const registry_parameter_t *parameter = node->value.parameter.parameter;

    parameter->schema->mapping(parameter->id, node->value.parameter.instance, &intern_val, &intern_val_len);

    if (buf_len > intern_val_len) {
        return -EINVAL;
    }

    /* call handler to apply the new value to the correct parameter in the instance of the schema */
    memcpy(intern_val, buf, buf_len);

    return 0;
}

static int _commit_export_cb(const registry_node_t *node, const void *context) {
    (void)context;

    const registry_instance_t *instance;

    switch (node->type)
    {
    /* The commit function is only called for instance and below */
    case REGISTRY_NODE_NAMESPACE:
    case REGISTRY_NODE_SCHEMA:
        return 0;

    case REGISTRY_NODE_INSTANCE:
        instance = node->value.instance;
        return instance->commit_cb(REGISTRY_COMMIT_INSTANCE, NULL, instance->context);

    case REGISTRY_NODE_GROUP:
        instance = node->value.group.instance;
        return instance->commit_cb(REGISTRY_COMMIT_GROUP, &node->value.group.group->id, instance->context);

    case REGISTRY_NODE_PARAMETER:
        instance = node->value.parameter.instance;
        return instance->commit_cb(REGISTRY_COMMIT_PARAMETER, &node->value.parameter.parameter->id, instance->context);
    }

    return 0;
}

int registry_commit(const registry_node_t *node) {
    int tree_traversal_depth = REGISTRY_EXPORT_WITH_N_LEVELS_OF_CHILDREN(3);

    if (node != NULL) {
        switch (node->type)
        {
        case REGISTRY_NODE_NAMESPACE:
            tree_traversal_depth = REGISTRY_EXPORT_WITH_N_LEVELS_OF_CHILDREN(2);
            break;

        case REGISTRY_NODE_SCHEMA:
            tree_traversal_depth = REGISTRY_EXPORT_WITH_N_LEVELS_OF_CHILDREN(1);
            break;

        case REGISTRY_NODE_INSTANCE:
        case REGISTRY_NODE_GROUP:
        case REGISTRY_NODE_PARAMETER:
            tree_traversal_depth = REGISTRY_EXPORT_SELF;
            break;
        }
    }

    return registry_export(node, _commit_export_cb, tree_traversal_depth, NULL);
}

static int _registry_export_parameter(const registry_instance_t *instance,
                              const registry_parameter_t *parameter,
                              const registry_export_cb_t export_cb, const void *context)
{
    assert(parameter != NULL);

    const registry_node_t export_node = {
        .type = REGISTRY_NODE_PARAMETER,
        .value.parameter = {
            .instance = instance,
            .parameter = parameter,
        },
    };

    return export_cb(&export_node, context);
}

static int _registry_export_group(const registry_instance_t *instance, const registry_group_t *group,
                          const registry_export_cb_t export_cb,
                          const uint8_t tree_traversal_depth, const void *context)
{
    assert(group != NULL);

    /* export the given configuration group */
    const registry_node_t export_node = {
        .type = REGISTRY_NODE_GROUP,
        .value.group = {
            .instance = instance,
            .group = group,
        },
    };
    int rc = export_cb(&export_node, context);

    /* export all children of the given configuration group if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return 0;
    }
    else {
        int new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        int _rc = rc;

        /* group */
        for (size_t i = 0; i < group->groups_len; i++) {
            _rc = _registry_export_group(instance, group->groups[i], export_cb, new_tree_traversal_depth,
                                        context);

            if (!_rc) {
                rc = _rc;
            }
        }

        /* parameter */
        for (size_t i = 0; i < group->parameters_len; i++) {
            _rc = _registry_export_parameter(instance, group->parameters[i], export_cb, context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

static int _registry_export_instance(const registry_instance_t *instance,
                             const registry_export_cb_t export_cb, const uint8_t tree_traversal_depth,
                             const void *context)
{
    assert(instance != NULL);

    /* export the given configuration schema instance */
    const registry_node_t export_node = {
        .type = REGISTRY_NODE_INSTANCE,
        .value.instance = instance,
    };
    int rc = export_cb(&export_node, context);

    /* export all groups or parameters of the given configuration schema instance if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return 0;
    }
    else {
        int new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        int _rc = rc;

        /* groups */
        for (size_t i = 0; i < instance->schema->groups_len; i++) {
            _rc = _registry_export_group(instance, instance->schema->groups[i], export_cb,
                                        new_tree_traversal_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        }

        /* parameters */
        for (size_t i = 0; i < instance->schema->parameters_len; i++) {
            _rc = _registry_export_parameter(instance, instance->schema->parameters[i], export_cb,
                                            context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

static int _registry_export_schema(const registry_schema_t *schema, const registry_export_cb_t export_cb,
                           const uint8_t tree_traversal_depth, const void *context)
{
    assert(schema != NULL);

    /* export the given configuration schema */
    const registry_node_t export_node = {
        .type = REGISTRY_NODE_SCHEMA,
        .value.schema = schema,
    };
    int rc = export_cb(&export_node, context);

    /* export all instances of the given configuration schema if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return 0;
    }
    else {
        int new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
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

            int _rc = _registry_export_instance(instance, export_cb, new_tree_traversal_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        } while (node != schema->instances.next);
    }

    return rc;
}

static int _registry_export_namespace(const registry_namespace_t *namespace,
                              const registry_export_cb_t export_cb, const uint8_t tree_traversal_depth,
                              const void *context)
{
    assert(namespace != NULL);

    /* export the given namespace */
    const registry_node_t export_node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .value.namespace = namespace,
    };
    int rc = export_cb(&export_node, context);

    /* export all configuration schemas of the given namespace if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return 0;
    }
    else {
        int new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        for (size_t i = 0; i < namespace->schemas_len; i++) {
            const registry_schema_t *child = namespace->schemas[i];

            int _rc = _registry_export_schema(child, export_cb, new_tree_traversal_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        }
    }

    return rc;
}

int registry_export(const registry_node_t *node, const registry_export_cb_t export_cb, const uint8_t tree_traversal_depth, const void *context)
{

    int rc = 0;

    if (node == NULL) {
        /* export all namespaces */
        for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
            registry_namespace_t *namespace = _registry_namespaces_xfa[i];

            int _rc = _registry_export_namespace(namespace, export_cb, tree_traversal_depth, context);

            if (!_rc) {
                rc = _rc;
            }
        }
    } else {
        switch (node->type)
        {
        case REGISTRY_NODE_NAMESPACE:
            rc = _registry_export_namespace(node->value.namespace, export_cb, tree_traversal_depth, context);
            break;
        case REGISTRY_NODE_SCHEMA:
            rc = _registry_export_schema(node->value.schema, export_cb, tree_traversal_depth, context);
            break;
        case REGISTRY_NODE_INSTANCE:
            rc = _registry_export_instance(node->value.instance, export_cb, tree_traversal_depth, context);
            break;
        case REGISTRY_NODE_GROUP:
            rc = _registry_export_group(node->value.group.instance, node->value.group.group, export_cb, tree_traversal_depth, context);
            break;
        case REGISTRY_NODE_PARAMETER:
            rc = _registry_export_parameter(node->value.parameter.instance, node->value.parameter.parameter, export_cb, context);
            break;
        }
    }

    return rc;
}
