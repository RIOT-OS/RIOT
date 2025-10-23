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
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"

#include "registry.h"
#include "registry/error.h"
#include "registry/find.h"

XFA_USE_CONST(registry_namespace_t *, _registry_namespaces_xfa);

static registry_find_result_type _find_group(
    const registry_group_t **groups,
    const size_t groups_len,
    const registry_find_comparator_t compare,
    const void *context,
    const registry_group_t **group)
{
    assert(groups != NULL);
    assert(groups_len > 0);
    assert(compare != NULL);

    registry_find_result_type res;
    registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
    };

    for (size_t i = 0; i < groups_len; i++) {
        node.value.group.group = groups[i];
        res = compare(&node, context);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            *group = groups[i];
            return res;
        }

        /* if a partial match is found, we need to keep searching for its children, but can break the loop */
        if (res == REGISTRY_FIND_PARTIAL_MATCH) {
            return _find_group(groups[i]->groups, groups[i]->groups_len, compare, context, group);
        }
    }

    return -REGISTRY_ERROR_GROUP_NOT_FOUND;
}

static registry_find_result_type _find_parameter_by_group(
    const registry_group_t *group,
    const registry_find_comparator_t compare,
    const void *context,
    const registry_parameter_t **parameter)
{
    assert(group != NULL);
    assert(compare != NULL);

    registry_find_result_type res;
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
    };

    /* search parameter at group root */
    for (size_t i = 0; i < group->parameters_len; i++) {
        node.value.parameter.parameter = group->parameters[i];
        res = compare(&node, context);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            *parameter = group->parameters[i];
            return res;
        }
    }

    /* search parameter inside sub-groups */
    for (size_t i = 0; i < group->groups_len; i++) {
        res = _find_parameter_by_group(group->groups[i], compare, context, parameter);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            return res;
        }
    }

    return -REGISTRY_ERROR_PARAMETER_NOT_FOUND;
}

static registry_find_result_type _find_parameter_by_schema(
    const registry_schema_t *schema,
    const registry_find_comparator_t compare,
    const void *context,
    const registry_parameter_t **parameter)
{
    assert(schema != NULL);
    assert(compare != NULL);

    registry_find_result_type res;
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
    };

    /* search parameter at schema root */
    for (size_t i = 0; i < schema->parameters_len; i++) {
        node.value.parameter.parameter = schema->parameters[i];
        res = compare(&node, context);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            *parameter = schema->parameters[i];
            return res;
        }
    }

    /* search parameter inside groups */
    for (size_t i = 0; i < schema->groups_len; i++) {
        res = _find_parameter_by_group(schema->groups[i], compare, context, parameter);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            return res;
        }
    }

    return -REGISTRY_ERROR_PARAMETER_NOT_FOUND;
}

registry_error_t registry_find(
    const registry_find_comparator_t compare,
    const void *context,
    registry_node_t *node)
{
    assert(compare != NULL);
    assert(context != NULL);

    registry_find_result_type res = REGISTRY_FIND_NO_MATCH;
    registry_node_t compare_node;

    /* Namespace */
    const registry_namespace_t *namespace;
    for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
        namespace = _registry_namespaces_xfa[i];
        compare_node.type = REGISTRY_NODE_NAMESPACE;
        compare_node.value.namespace = namespace;
        res = compare(&compare_node, context);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            node->type = REGISTRY_NODE_NAMESPACE;
            node->value.namespace = namespace;
            return 0;
        }

        /* if a partial match is found, we need to keep searching for its children, but can break the loop */
        if (res == REGISTRY_FIND_PARTIAL_MATCH) {
            break;
        }
    }

    /* Check if a namespace was found */
    if (res == REGISTRY_FIND_NO_MATCH) {
        return -REGISTRY_ERROR_NAMESPACE_NOT_FOUND;
    }

    /* Schema */
    const registry_schema_t *schema = NULL;
    for (size_t i = 0; i < namespace->schemas_len; i++) {
        schema = namespace->schemas[i];
        compare_node.type = REGISTRY_NODE_SCHEMA;
        compare_node.value.schema = schema;
        res = compare(&compare_node, context);

        /* if an exact match is found, we can set it and return the function */
        if (res == REGISTRY_FIND_EXACT_MATCH) {
            node->type = REGISTRY_NODE_SCHEMA;
            node->value.schema = schema;
            return 0;
        }

        /* if a partial match is found, we need to keep searching for its children, but can break the loop */
        if (res == REGISTRY_FIND_PARTIAL_MATCH) {
            break;
        }
    }

    /* Check if a schema was found */
    if (res == REGISTRY_FIND_NO_MATCH) {
        return -REGISTRY_ERROR_SCHEMA_NOT_FOUND;
    }

    if (schema != NULL) {
        /* Instance */
        const registry_instance_t *instance = NULL;
        clist_node_t *instance_node = schema->instances.next;
        if (instance_node) {
            do {
                instance_node = instance_node->next;
                instance = container_of(instance_node, registry_instance_t, node);
                compare_node.type = REGISTRY_NODE_INSTANCE;
                compare_node.value.instance = instance;

                res = compare(&compare_node, context);

                /* if an exact match is found, we can set it and return the function */
                if (res == REGISTRY_FIND_EXACT_MATCH) {
                    node->type = REGISTRY_NODE_INSTANCE;
                    node->value.instance = instance;
                    return 0;
                }

                /* if a partial match is found, we need to keep searching for its children, but can break the loop */
                if (res == REGISTRY_FIND_PARTIAL_MATCH) {
                    break;
                }
            } while (instance_node != schema->instances.next);
        }

        /* Check if an instance was found */
        if (res == REGISTRY_FIND_NO_MATCH) {
            return -REGISTRY_ERROR_INSTANCE_NOT_FOUND;
        }

        if (instance != NULL) {
            /* Group */
            if (schema->groups != NULL && schema->groups_len > 0) {
                const registry_group_t *group;
                res = _find_group(schema->groups, schema->groups_len, compare, context, &group);
                /* if an exact match is found, we can set it and return the function */
                if (res == REGISTRY_FIND_EXACT_MATCH) {
                    node->type = REGISTRY_NODE_GROUP;
                    node->value.group.group = group;
                    node->value.group.instance = instance;
                    return 0;
                }

                /* if a partial match is found, we need to keep searching for its children */
                if (res == REGISTRY_FIND_PARTIAL_MATCH) {
                    const registry_parameter_t *parameter;
                    res = _find_parameter_by_group(node->value.group.group, compare, context,
                                                   &parameter);

                    if (res == REGISTRY_FIND_EXACT_MATCH) {
                        node->type = REGISTRY_NODE_PARAMETER;
                        node->value.parameter.parameter = parameter;
                        node->value.parameter.instance = instance;
                        return 0;
                    }

                    /* A partial match of a parameter cannot happen as it is a leaf of the configuration tree */
                    return -REGISTRY_ERROR_PARAMETER_NOT_FOUND;
                }
            }

            /* Parameter */
            const registry_parameter_t *parameter;
            res = _find_parameter_by_schema(schema, compare, context, &parameter);
            /* if an exact match is found, we can set it and return the function */
            if (res == REGISTRY_FIND_EXACT_MATCH) {
                node->type = REGISTRY_NODE_PARAMETER;
                node->value.parameter.parameter = parameter;
                node->value.parameter.instance = instance;
                return 0;
            }

            /* A partial match of a parameter cannot happen as it is a leaf of the configuration tree */
        }
    }

    return -REGISTRY_ERROR_PARAMETER_NOT_FOUND;
}
