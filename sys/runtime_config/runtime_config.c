/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_runtime_config
 * @brief       Runtime configuration for runtime configuration of modules
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

#define ENABLE_DEBUG 0
#include "debug.h"

XFA_INIT_CONST(runtime_config_namespace_t *, _runtime_config_namespaces_xfa);

runtime_config_error_t runtime_config_add_schema_instance(
    runtime_config_schema_t *schema,
    runtime_config_schema_instance_t *schema_instance)
{
    assert(schema != NULL);
    assert(schema_instance != NULL);

    /* add schema to instance */
    schema_instance->schema = schema;

    /* get instances length to determine the id of the new instance */
    size_t count = clist_count(&schema->instances);

    /* set id of new instance to the instance count */
    schema_instance->id = count;

    /* add instance to schema */
    clist_rpush(&schema->instances, &schema_instance->node);

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_get(
    const runtime_config_node_t *node,
    void **buf,
    size_t *buf_len)
{
    assert(node != NULL);
    assert(buf != NULL);
    assert(buf_len != NULL);

    if (node->type != RUNTIME_CONFIG_NODE_TYPE_PARAMETER) {
        return RUNTIME_CONFIG_ERROR_NODE_INVALID;
    }

    /* get pointer to internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const runtime_config_parameter_t *parameter = node->as_parameter.parameter;

    parameter->schema->get_parameter_value_from_instance(
        parameter->id, node->as_parameter.schema_instance, &intern_val, &intern_val_len);

    /* update buf pointer to point to its internal value
     * and set buf_len accordingly */
    *buf = intern_val;
    *buf_len = intern_val_len;

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_set(
    const runtime_config_node_t *node,
    const void *buf,
    const size_t buf_len)
{
    assert(node != NULL);
    assert(buf != NULL);
    assert(buf_len > 0);

    if (node->type != RUNTIME_CONFIG_NODE_TYPE_PARAMETER) {
        return RUNTIME_CONFIG_ERROR_NODE_INVALID;
    }

    /* get pointer to the internal value buffer and length */
    void *intern_val = NULL;
    size_t intern_val_len;

    const runtime_config_parameter_t *parameter = node->as_parameter.parameter;

    parameter->schema->get_parameter_value_from_instance(
        parameter->id, node->as_parameter.schema_instance, &intern_val, &intern_val_len);

    if (buf_len != intern_val_len) {
        if (buf_len > intern_val_len) {
            return RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_LARGE;
        }

        if (buf_len < intern_val_len &&
            parameter->type != RUNTIME_CONFIG_TYPE_BYTES &&
            parameter->type != RUNTIME_CONFIG_TYPE_STRING) {
            return RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_SMALL;
        }
    }

    /* copy buffer to apply the new value to the correct instance of the schema */
    memcpy(intern_val, buf, buf_len);

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t _apply_tree_traversal_cb(
    const runtime_config_node_t *node,
    const void *context)
{
    (void)context;

    assert(node != NULL);

    const runtime_config_schema_instance_t *schema_instance;

    switch (node->type) {
    /* The apply function is only called for instance and below */
    case RUNTIME_CONFIG_NODE_TYPE_NAMESPACE:
    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA:
        return RUNTIME_CONFIG_ERROR_NONE;

    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE:
        schema_instance = node->as_schema_instance;
        return schema_instance->apply_schema_instance_cb(schema_instance);

    case RUNTIME_CONFIG_NODE_TYPE_GROUP:
        schema_instance = node->as_group.schema_instance;
        return schema_instance->apply_group_or_parameter_cb(
            schema_instance, node->as_group.group->id);

    case RUNTIME_CONFIG_NODE_TYPE_PARAMETER:
        schema_instance = node->as_parameter.schema_instance;
        return schema_instance->apply_group_or_parameter_cb(
            schema_instance, node->as_parameter.parameter->id);
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

runtime_config_error_t runtime_config_apply(const runtime_config_node_t *node)
{
    uint8_t tree_traversal_depth = RUNTIME_CONFIG_TRAVERSE_TREE_WITH_N_LEVELS_OF_CHILDREN(3);

    if (node != NULL) {
        switch (node->type) {
        case RUNTIME_CONFIG_NODE_TYPE_NAMESPACE:
            tree_traversal_depth = RUNTIME_CONFIG_TRAVERSE_TREE_WITH_N_LEVELS_OF_CHILDREN(2);
            break;

        case RUNTIME_CONFIG_NODE_TYPE_SCHEMA:
            tree_traversal_depth = RUNTIME_CONFIG_TRAVERSE_TREE_WITH_N_LEVELS_OF_CHILDREN(1);
            break;

        case RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE:
        case RUNTIME_CONFIG_NODE_TYPE_GROUP:
        case RUNTIME_CONFIG_NODE_TYPE_PARAMETER:
            tree_traversal_depth = RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE;
            break;
        }
    }

    return runtime_config_traverse_config_tree(
        node,
        _apply_tree_traversal_cb,
        tree_traversal_depth,
        NULL);
}

static runtime_config_error_t _runtime_config_traverse_parameter_tree(
    const runtime_config_schema_instance_t *schema_instance,
    const runtime_config_parameter_t *parameter,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const void *context)
{
    assert(parameter != NULL);
    assert(schema_instance != NULL);

    const runtime_config_node_t tree_traversal_node = RUNTIME_CONFIG_NODE_PARAMETER(
        schema_instance, parameter);

    return tree_traversal_cb(&tree_traversal_node, context);
}

static runtime_config_error_t _runtime_config_traverse_group_tree(
    const runtime_config_schema_instance_t *schema_instance,
    const runtime_config_group_t *group,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(group != NULL);
    assert(schema_instance != NULL);

    /* return the given configuration group */
    const runtime_config_node_t tree_traversal_node = RUNTIME_CONFIG_NODE_GROUP(schema_instance, group);
    runtime_config_error_t rc = tree_traversal_cb(&tree_traversal_node, context);

    /* traverse through all children of the given configuration group
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        /* group */
        for (size_t i = 0; i < group->groups_len; i++) {
            rc = _runtime_config_traverse_group_tree(
                schema_instance, group->groups[i], tree_traversal_cb,
                tree_traversal_depth - 1, context);

            if (rc != RUNTIME_CONFIG_ERROR_NONE) {
                return rc;
            }
        }

        /* parameter */
        for (size_t i = 0; i < group->parameters_len; i++) {
            rc = _runtime_config_traverse_parameter_tree(
                schema_instance, group->parameters[i], tree_traversal_cb, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

static runtime_config_error_t _runtime_config_traverse_schema_tree_instance(
    const runtime_config_schema_instance_t *schema_instance,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(schema_instance != NULL);

    /* return the given configuration schema instance */
    const runtime_config_node_t tree_traversal_node = RUNTIME_CONFIG_NODE_SCHEMA_INSTANCE(schema_instance);
    runtime_config_error_t rc = tree_traversal_cb(&tree_traversal_node, context);

    /* traverse through all groups or parameters of the given configuration
     * schema instance if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        /* groups */
        for (size_t i = 0; i < schema_instance->schema->groups_len; i++) {
            rc = _runtime_config_traverse_group_tree(
                schema_instance, schema_instance->schema->groups[i], tree_traversal_cb,
                tree_traversal_depth - 1, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }

        /* parameters */
        for (size_t i = 0; i < schema_instance->schema->parameters_len; i++) {
            rc = _runtime_config_traverse_parameter_tree(
                schema_instance, schema_instance->schema->parameters[i], tree_traversal_cb, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

static runtime_config_error_t _runtime_config_traverse_schema_tree(
    const runtime_config_schema_t *schema,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(schema != NULL);

    /* return the given configuration schema */
    const runtime_config_node_t tree_traversal_node = RUNTIME_CONFIG_NODE_SCHEMA(schema);
    runtime_config_error_t rc = tree_traversal_cb(&tree_traversal_node, context);

    /* traverse through all instances of the given configuration schema
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        clist_node_t *node = schema->instances.next;

        if (!node) {
            return RUNTIME_CONFIG_ERROR_NONE;
        }

        do {
            node = node->next;
            runtime_config_schema_instance_t *schema_instance = container_of(
                node,
                runtime_config_schema_instance_t,
                node);

            rc = _runtime_config_traverse_schema_tree_instance(
                schema_instance,
                tree_traversal_cb,
                tree_traversal_depth - 1,
                context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        } while (node != schema->instances.next);
    }

    return rc;
}

static runtime_config_error_t _runtime_config_traverse_namespace_tree(
    const runtime_config_namespace_t *namespace,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    assert(namespace != NULL);

    /* return the given namespace */
    const runtime_config_node_t tree_traversal_node = RUNTIME_CONFIG_NODE_NAMESPACE(namespace);
    runtime_config_error_t rc = tree_traversal_cb(&tree_traversal_node, context);

    /* traverse through all configuration schemas of the given namespace
     * if available and within tree_traversal_depth bounds */
    if (tree_traversal_depth == RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE) {
        return RUNTIME_CONFIG_ERROR_NONE;
    }
    else {
        for (size_t i = 0; i < namespace->schemas_len; i++) {
            const runtime_config_schema_t *child = namespace->schemas[i];

            rc = _runtime_config_traverse_schema_tree(
                child, tree_traversal_cb, tree_traversal_depth - 1, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
    }

    return rc;
}

runtime_config_error_t runtime_config_traverse_config_tree(
    const runtime_config_node_t *node,
    const runtime_config_tree_traversal_cb_t tree_traversal_cb,
    const uint8_t tree_traversal_depth,
    const void *context)
{
    runtime_config_error_t rc = RUNTIME_CONFIG_ERROR_NONE;

    if (node == NULL) {
        /* don't return anything if tree_traversal depth is 1,
         * because 1 means only return the exact match and that would be NULL */
        if (tree_traversal_depth == RUNTIME_CONFIG_TRAVERSE_SINGLE_NODE) {
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        /* iterate through all namespaces */
        for (size_t i = 0; i < XFA_LEN(runtime_config_namespace_t *, _runtime_config_namespaces_xfa); i++) {
            runtime_config_namespace_t *namespace = _runtime_config_namespaces_xfa[i];

            /* we write tree_traversal_depth - 1, because we already iterated over namespaces */
            rc = _runtime_config_traverse_namespace_tree(
                namespace, tree_traversal_cb,
                tree_traversal_depth - 1, context);

            if (!(rc == RUNTIME_CONFIG_ERROR_NONE)) {
                return rc;
            }
        }
        return rc;
    }

    switch (node->type) {
    case RUNTIME_CONFIG_NODE_TYPE_NAMESPACE:
        rc = _runtime_config_traverse_namespace_tree(
            node->as_namespace, tree_traversal_cb, tree_traversal_depth, context);
        break;
    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA:
        rc = _runtime_config_traverse_schema_tree(
            node->as_schema, tree_traversal_cb, tree_traversal_depth, context);
        break;
    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE:
        rc = _runtime_config_traverse_schema_tree_instance(
            node->as_schema_instance, tree_traversal_cb, tree_traversal_depth, context);
        break;
    case RUNTIME_CONFIG_NODE_TYPE_GROUP:
        rc = _runtime_config_traverse_group_tree(
            node->as_group.schema_instance, node->as_group.group, tree_traversal_cb,
            tree_traversal_depth, context);
        break;
    case RUNTIME_CONFIG_NODE_TYPE_PARAMETER:
        rc = _runtime_config_traverse_parameter_tree(
            node->as_parameter.schema_instance, node->as_parameter.parameter,
            tree_traversal_cb, context);
        break;
    }

    return rc;
}
