/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_runtime_config
 * @brief       Runtime config for runtime configuration of modules
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

#include "kernel_defines.h"
#include "clist.h"

#include "runtime_config.h"
#include "runtime_config/util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

XFA_INIT_CONST(runtime_config_namespace_t *, _runtime_config_namespaces_xfa);

runtime_config_error_t runtime_config_add_schema_instance(
    const runtime_config_schema_t *schema,
    const runtime_config_schema_instance_t *instance)
{
    assert(schema != NULL);
    assert(instance != NULL);

    /* add schema to instance */
    ((runtime_config_schema_instance_t *)instance)->schema = schema;

    /* get instances length to determine the id of the new instance */
    size_t count = clist_count((clist_node_t *)&schema->instances);

    /* set id of new instance to the instance count */
    *(runtime_config_schema_instance_id_t *)&instance->id = count;

    /* add instance to schema */
    clist_rpush((clist_node_t *)&schema->instances, (clist_node_t *)&instance->node);

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_get(const runtime_config_node_t *node, runtime_config_value_t *value)
{
    assert(node != NULL);
    assert(value != NULL);

    if (node->type != RUNTIME_CONFIG_NODE_PARAMETER) {
        return -RUNTIME_CONFIG_ERROR_NODE_INVALID;
    }

    /* get pointer to runtime config internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const runtime_config_parameter_t *parameter = node->value.parameter.parameter;

    parameter->schema->get_parameter_value_from_instance(
        parameter->id, node->value.parameter.instance, &intern_val, &intern_val_len);

    /* update buf pointer in runtime_config_value_t to point
     * to its internal value and set buf_len accordingly */
    value->count = parameter->count;
    value->type = parameter->type;
    value->buf = intern_val;
    value->buf_len = intern_val_len;

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_set(const runtime_config_node_t *node, const void *buf, const size_t buf_len)
{
    assert(node != NULL);
    assert(buf != NULL);

    if (node->type != RUNTIME_CONFIG_NODE_PARAMETER) {
        return -RUNTIME_CONFIG_ERROR_NODE_INVALID;
    }

    /* get pointer to the runtime config internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const runtime_config_parameter_t *parameter = node->value.parameter.parameter;

    parameter->schema->get_parameter_value_from_instance(
        parameter->id, node->value.parameter.instance, &intern_val, &intern_val_len);

    if (buf_len > intern_val_len) {
        return -RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_LARGE;
    }

    /* copy buffer to apply the new value to the correct instance of the schema */
    memcpy(intern_val, buf, buf_len);

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t _apply_export_cb(const runtime_config_node_t *node, const void *context)
{
    (void)context;
   assert(node != NULL);
    const runtime_config_schema_instance_t *instance;

    switch (node->type) {
    /* The apply function is only called for instance and below */
    case RUNTIME_CONFIG_NODE_NAMESPACE:
    case RUNTIME_CONFIG_NODE_SCHEMA:
        return RUNTIME_CONFIG_ERROR_NONE;

    case RUNTIME_CONFIG_NODE_INSTANCE:
        instance = node->value.instance;
        return instance->apply_cb(NULL, instance);

    case RUNTIME_CONFIG_NODE_GROUP:
        instance = node->value.group.instance;
        return instance->apply_cb(
            &node->value.group.group->id,
            instance);

    case RUNTIME_CONFIG_NODE_PARAMETER:
        instance = node->value.parameter.instance;
        return instance->apply_cb(
            &node->value.parameter.parameter->id,
            instance);
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_apply(const runtime_config_node_t *node)
{
    uint8_t tree_traversal_depth = RUNTIME_CONFIG_EXPORT_WITH_N_LEVELS_OF_CHILDREN(3);

    if (node != NULL) {
        switch (node->type) {
        case RUNTIME_CONFIG_NODE_NAMESPACE:
            tree_traversal_depth = RUNTIME_CONFIG_EXPORT_WITH_N_LEVELS_OF_CHILDREN(2);
            break;

        case RUNTIME_CONFIG_NODE_SCHEMA:
            tree_traversal_depth = RUNTIME_CONFIG_EXPORT_WITH_N_LEVELS_OF_CHILDREN(1);
            break;

        case RUNTIME_CONFIG_NODE_INSTANCE:
        case RUNTIME_CONFIG_NODE_GROUP:
        case RUNTIME_CONFIG_NODE_PARAMETER:
            tree_traversal_depth = RUNTIME_CONFIG_EXPORT_SELF;
            break;
        }
    }

    return runtime_config_export(node, _apply_export_cb, tree_traversal_depth, NULL);
}

static runtime_config_error_t _runtime_config_export_parameter(
    const runtime_config_schema_instance_t *instance,
    const runtime_config_parameter_t *parameter,
    const runtime_config_export_cb_t export_cb,
    const void *context)
{
    assert(parameter != NULL);

    const runtime_config_node_t export_node = {
        .type = RUNTIME_CONFIG_NODE_PARAMETER,
        .value.parameter = {
            .instance = instance,
            .parameter = parameter,
        },
    };

    return export_cb(&export_node, context);
}

static runtime_config_error_t _runtime_config_export_group(
    const runtime_config_schema_instance_t *instance,
    const runtime_config_group_t *group,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(group != NULL);

    /* export the given configuration group */
    const runtime_config_node_t export_node = {
        .type = RUNTIME_CONFIG_NODE_GROUP,
        .value.group = {
            .instance = instance,
            .group = group,
        },
    };
    runtime_config_error_t rc = export_cb(&export_node, context);

    /* export all children of the given configuration group
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        uint8_t new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        /* group */
        for (size_t i = 0; i < group->groups_len; i++) {
            rc = _runtime_config_export_group(
                instance, group->groups[i], export_cb,
                new_tree_traversal_depth, context);

            if (rc != RUNTIME_CONFIG_ERROR_NONE) {
                return rc;
            }
        }

        /* parameter */
        for (size_t i = 0; i < group->parameters_len; i++) {
            rc = _runtime_config_export_parameter(
                instance, group->parameters[i], export_cb, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

static runtime_config_error_t _runtime_config_export_schema_instance(
    const runtime_config_schema_instance_t *instance,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(instance != NULL);

    /* export the given configuration schema instance */
    const runtime_config_node_t export_node = {
        .type = RUNTIME_CONFIG_NODE_INSTANCE,
        .value.instance = instance,
    };
    runtime_config_error_t rc = export_cb(&export_node, context);

    /* export all groups or parameters of the given configuration schema instance
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        uint8_t new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        /* groups */
        for (size_t i = 0; i < instance->schema->groups_len; i++) {
            rc = _runtime_config_export_group(
                instance, instance->schema->groups[i], export_cb,
                new_tree_traversal_depth, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }

        /* parameters */
        for (size_t i = 0; i < instance->schema->parameters_len; i++) {
            rc = _runtime_config_export_parameter(
                instance, instance->schema->parameters[i], export_cb, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

static runtime_config_error_t _runtime_config_export_schema(
    const runtime_config_schema_t *schema,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(schema != NULL);

    /* export the given configuration schema */
    const runtime_config_node_t export_node = {
        .type = RUNTIME_CONFIG_NODE_SCHEMA,
        .value.schema = schema,
    };
    runtime_config_error_t rc = export_cb(&export_node, context);

    /* export all instances of the given configuration schema
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        uint8_t new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        clist_node_t *node = schema->instances.next;

        if (!node) {
            return RUNTIME_CONFIG_ERROR_NONE;
        }

        do {
            node = node->next;
            runtime_config_schema_instance_t *instance = container_of(node, runtime_config_schema_instance_t, node);

            if (!instance) {
                return -RUNTIME_CONFIG_ERROR_SCHEMA_HAS_NO_INSTANCE;
            }

            rc = _runtime_config_export_schema_instance(instance, export_cb, new_tree_traversal_depth, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        } while (node != schema->instances.next);
    }

    return rc;
}

static runtime_config_error_t _runtime_config_export_namespace(
    const runtime_config_namespace_t *namespace,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(namespace != NULL);

    /* export the given namespace */
    const runtime_config_node_t export_node = {
        .type = RUNTIME_CONFIG_NODE_NAMESPACE,
        .value.namespace = namespace,
    };
    runtime_config_error_t rc = export_cb(&export_node, context);

    /* export all configuration schemas of the given namespace
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == 1) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        uint8_t new_tree_traversal_depth = tree_traversal_depth;
        if (tree_traversal_depth > 1) {
            new_tree_traversal_depth--;
        }

        for (size_t i = 0; i < namespace->schemas_len; i++) {
            const runtime_config_schema_t *child = namespace->schemas[i];

            rc = _runtime_config_export_schema(
                child, export_cb, new_tree_traversal_depth, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

runtime_config_error_t runtime_config_export(
    const runtime_config_node_t *node,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    runtime_config_error_t rc = RUNTIME_CONFIG_ERROR_NONE;

    if (node == NULL) {
        /* don't export anything if tree_traversal depth is 1,
         * because 1 means only export exact match and that would be NULL */
        if (tree_traversal_depth == 1) {
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        /* export all namespaces */
        for (size_t i = 0; i < XFA_LEN(runtime_config_namespace_t *, _runtime_config_namespaces_xfa); i++) {
            runtime_config_namespace_t *namespace = _runtime_config_namespaces_xfa[i];

            /* we write tree_traversal_depth - 1, because we already iterated over namespaces */
            rc = _runtime_config_export_namespace(namespace, export_cb, tree_traversal_depth - 1, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
        return rc;
    }

    switch (node->type) {
    case RUNTIME_CONFIG_NODE_NAMESPACE:
        rc = _runtime_config_export_namespace(node->value.namespace, export_cb, tree_traversal_depth,
                                              context);
        break;
    case RUNTIME_CONFIG_NODE_SCHEMA:
        rc = _runtime_config_export_schema(node->value.schema, export_cb, tree_traversal_depth,
                                           context);
        break;
    case RUNTIME_CONFIG_NODE_INSTANCE:
        rc = _runtime_config_export_schema_instance(node->value.instance, export_cb, tree_traversal_depth,
                                                    context);
        break;
    case RUNTIME_CONFIG_NODE_GROUP:
        rc = _runtime_config_export_group(node->value.group.instance, node->value.group.group,
                                          export_cb, tree_traversal_depth, context);
        break;
    case RUNTIME_CONFIG_NODE_PARAMETER:
        rc = _runtime_config_export_parameter(node->value.parameter.instance,
                                              node->value.parameter.parameter, export_cb, context);
        break;
    }

    return rc;
}
