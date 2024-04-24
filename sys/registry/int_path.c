/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_int_path RIOT Registry Int Path
 * @ingroup     sys
 * @brief       RIOT Registry Int Path module providing a API to access configuration parameter via an integer path
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "clist.h"
#include "registry.h"
#include "registry/util.h"
#include "registry/error.h"

#include "registry/int_path.h"

XFA_USE_CONST(registry_namespace_t *, _registry_namespaces_xfa);

static int _namespace_lookup(const registry_namespace_id_t namespace_id,
                             registry_namespace_t **namespace)
{
    assert(namespace != NULL);

    for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
        if (_registry_namespaces_xfa[i]->id == namespace_id) {
            *namespace = _registry_namespaces_xfa[i];
            return 0;
        }
    }

    return -REGISTRY_ERROR_NAMESPACE_NOT_FOUND;
}

static int _schema_lookup(const registry_namespace_t *namespace,
                          const registry_schema_id_t schema_id,
                          const registry_schema_t **schema)
{
    assert(schema != NULL);

    for (size_t i = 0; i < namespace->schemas_len; i++) {
        if (namespace->schemas[i]->id == schema_id) {
            *schema = namespace->schemas[i];
            return 0;
        }
    }

    return -REGISTRY_ERROR_SCHEMA_NOT_FOUND;
}

static int _instance_lookup(const registry_schema_t *schema,
                            const registry_instance_id_t instance_id,
                            registry_instance_t **instance)
{
    assert(schema != NULL);
    assert(instance != NULL);

    /* find instance with correct instance_id */
    clist_node_t *node = schema->instances.next;

    if (!node) {
        return -REGISTRY_ERROR_INSTANCE_NOT_FOUND;
    }

    do {
        node = node->next;

        /* check if index equals instance_id */
        if (container_of(node, registry_instance_t, node)->id == instance_id) {
            *instance = container_of(node, registry_instance_t, node);
            return 0;
        }
    } while (node != schema->instances.next);

    return -REGISTRY_ERROR_INSTANCE_NOT_FOUND;
}

static int _group_lookup(const registry_group_t **groups,
                         const size_t groups_len,
                         const registry_group_id_t group_id,
                         const registry_group_t **group)
{
    assert(groups != NULL);
    assert(groups_len > 0);

    for (size_t i = 0; i < groups_len; i++) {
        if ((*groups)[i].id == group_id) {
            *group = groups[i];
            return 0;
        }
        else if ((*groups)[i].groups_len > 0) {
            return _group_lookup((*groups)[i].groups, (*groups)[i].groups_len, group_id, group);
        }
    }

    return -REGISTRY_ERROR_GROUP_NOT_FOUND;
}

static const registry_parameter_t *_internal_parameter_lookup(const registry_group_t *group,
                                                              const registry_parameter_id_t parameter_id)
{
    assert(group != NULL);

    for (size_t i = 0; i < group->parameters_len; i++) {
        if (group->parameters[i]->id == parameter_id) {
            return group->parameters[i];
        }
    }

    for (size_t i = 0; i < group->groups_len; i++) {
        return _internal_parameter_lookup(group->groups[i], parameter_id);
    }

    return NULL;
}

static int _parameter_lookup(const registry_schema_t *schema,
                             const registry_parameter_id_t parameter_id,
                             const registry_parameter_t **parameter)
{
    assert(schema != NULL);
    assert(parameter != NULL);

    for (size_t i = 0; i < schema->parameters_len; i++) {
        if (schema->parameters[i]->id == parameter_id) {
            *parameter = schema->parameters[i];
            return 0;
        }
    }

    for (size_t i = 0; i < schema->groups_len; i++) {
        const registry_parameter_t *found_parameter = _internal_parameter_lookup(schema->groups[i], parameter_id);
        if (found_parameter != NULL) {
            *parameter = found_parameter;
            return 0;
        }
    }

    return -REGISTRY_ERROR_PARAMETER_NOT_FOUND;
}

int registry_node_to_int_path(const registry_node_t *node, registry_int_path_t *path)
{
    assert(node != NULL);
    assert(path != NULL);

    switch (node->type)
    {
    case REGISTRY_NODE_NAMESPACE:
        path->type = REGISTRY_INT_PATH_TYPE_NAMESPACE;
        path->value.namespace_path.namespace_id = node->value.namespace->id;
        break;
    
    case REGISTRY_NODE_SCHEMA:
        path->type = REGISTRY_INT_PATH_TYPE_SCHEMA;
        path->value.schema_path.namespace_id = node->value.schema->namespace->id;
        path->value.schema_path.schema_id = node->value.schema->id;
        break;
    
    case REGISTRY_NODE_INSTANCE:
        path->type = REGISTRY_INT_PATH_TYPE_INSTANCE;
        path->value.instance_path.namespace_id = node->value.instance->schema->namespace->id;
        path->value.instance_path.schema_id = node->value.instance->schema->id;
        path->value.instance_path.instance_id = node->value.instance->id;
        break;
    
    case REGISTRY_NODE_GROUP:
        path->type = REGISTRY_INT_PATH_TYPE_GROUP;
        path->value.group_path.namespace_id = node->value.group.group->schema->namespace->id;
        path->value.group_path.schema_id = node->value.group.group->schema->id;
        path->value.group_path.instance_id = node->value.group.instance->id;
        path->value.group_path.group_id = node->value.group.group->id;
        break;
    
    case REGISTRY_NODE_PARAMETER:
        path->type = REGISTRY_INT_PATH_TYPE_PARAMETER;
        path->value.parameter_path.namespace_id = node->value.parameter.parameter->schema->namespace->id;
        path->value.parameter_path.schema_id = node->value.parameter.parameter->schema->id;
        path->value.parameter_path.instance_id = node->value.parameter.instance->id;
        path->value.parameter_path.parameter_id = node->value.parameter.parameter->id;
        break;
    }

    return 0;
}

int registry_node_from_int_path(const registry_int_path_t *path, registry_node_t *node)
{
    assert(path != NULL);
    assert(node != NULL);

    registry_namespace_id_t namespace_id = 0;
    registry_schema_id_t schema_id = 0;
    registry_instance_id_t instance_id = 0;
    registry_group_or_parameter_id_t group_or_parameter_id = 0;
    registry_parameter_id_t parameter_id = 0;
    registry_group_id_t group_id = 0;

    switch (path->type)
    {
    case REGISTRY_INT_PATH_TYPE_NAMESPACE:
        namespace_id = path->value.namespace_path.namespace_id;
        break;

    case REGISTRY_INT_PATH_TYPE_SCHEMA:
        namespace_id = path->value.schema_path.namespace_id;
        schema_id = path->value.schema_path.schema_id;
        break;

    case REGISTRY_INT_PATH_TYPE_INSTANCE:
        namespace_id = path->value.instance_path.namespace_id;
        schema_id = path->value.instance_path.schema_id;
        instance_id = path->value.instance_path.instance_id;
        break;

    case REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER:
        namespace_id = path->value.group_or_parameter_path.namespace_id;
        schema_id = path->value.group_or_parameter_path.schema_id;
        instance_id = path->value.group_or_parameter_path.instance_id;
        group_or_parameter_id = path->value.group_or_parameter_path.group_or_parameter_id;
        break;

    case REGISTRY_INT_PATH_TYPE_GROUP:
        namespace_id = path->value.group_path.namespace_id;
        schema_id = path->value.group_path.schema_id;
        instance_id = path->value.group_path.instance_id;
        group_id = path->value.group_path.group_id;
        break;

    case REGISTRY_INT_PATH_TYPE_PARAMETER:
        namespace_id = path->value.parameter_path.namespace_id;
        schema_id = path->value.parameter_path.schema_id;
        instance_id = path->value.parameter_path.instance_id;
        parameter_id = path->value.parameter_path.parameter_id;
        break;
    }

    /* Namespace */
    registry_namespace_t *namespace;
    int res = _namespace_lookup(namespace_id, &namespace);

    if (res == 0) {
        if (path->type == REGISTRY_INT_PATH_TYPE_NAMESPACE) {
            node->type = REGISTRY_NODE_NAMESPACE;
            node->value.namespace = namespace;
            return res;
        }
    
        /* Schema */
        const registry_schema_t *schema;
        res = _schema_lookup(namespace, schema_id, &schema);

        if (res == 0) {
            if (path->type == REGISTRY_INT_PATH_TYPE_SCHEMA) {
                node->type = REGISTRY_NODE_SCHEMA;
                node->value.schema = schema;
                return res;
            }
            
            /* Instance */
            registry_instance_t *instance;
            res = _instance_lookup(schema, instance_id, &instance);

            if (res == 0) {
                if (path->type == REGISTRY_INT_PATH_TYPE_INSTANCE) {
                    node->type = REGISTRY_NODE_INSTANCE;
                    node->value.instance = instance;
                    return res;
                }

                /* Group or Parameter */
                if (path->type == REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER) {
                    res = _group_lookup(schema->groups, schema->groups_len, group_or_parameter_id, &node->value.group.group);
                    if (res == 0) {
                        node->type = REGISTRY_NODE_GROUP;
                        node->value.group.instance = instance;
                        return res;
                    }
                    
                    res = _parameter_lookup(schema, group_or_parameter_id, &node->value.parameter.parameter);
                    if (res == 0) {
                        node->type = REGISTRY_NODE_PARAMETER;
                        node->value.parameter.instance = instance;
                        return res;
                    }
                }
                
                /* Group */
                if (path->type == REGISTRY_INT_PATH_TYPE_GROUP) {
                    node->type = REGISTRY_NODE_GROUP;
                    node->value.group.instance = instance;
                    return _group_lookup(schema->groups, schema->groups_len, group_id, &node->value.group.group);
                }

                /* Parameter */
                if (path->type == REGISTRY_INT_PATH_TYPE_PARAMETER) {
                    node->type = REGISTRY_NODE_PARAMETER;
                    node->value.parameter.instance = instance;
                    return _parameter_lookup(schema, parameter_id, &node->value.parameter.parameter);
                }
            }
        }
    }

    return res;
}
