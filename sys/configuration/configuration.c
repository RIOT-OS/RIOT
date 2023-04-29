/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_configuration
 * @{
 *
 * @file
 * @brief       Implementation of a runtime configuration module
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#include "fmt.h"
#include "modules.h"
#include "container.h"
#include "configuration.h"
#include "mutex.h"

/**
 * @brief   Iterator type to iterate over the configuration tree non-recursively
 */
typedef struct configuration_iterator {
    /**
     * @brief   Root node to start from
     */
    const conf_handler_node_t *root;
    /**
     * @brief   Stack pointer
     */
    unsigned short sp;
    /**
     * @brief  Whether to retrieve all subhandlers or just the first one
     */
    bool max_depth;
    /**
     * @brief   Stack of handlers for iteration
     */
    const conf_handler_node_t *stack[CONFIGURATION_DEPTH_MAX + 1];
} conf_iterator_t;

static conf_handler_node_t _conf_root_handler
    = CONF_HANDLER_NODE_INITIALIZER("/");

static char *_configuration_key_next(char *key, const char *subkey)
{
    assert(key);
    assert(subkey);
    int len = strlen(subkey);
    /* not a subkey or not root and subkey does not match a full segment */
    if (strncmp(key, subkey, len) || (*key != '/' && key[len] && key[len] != '/')) {
        return NULL;
    }
    key += len;
    while (*key == '/') {
        key++;
    }
    return key;
}

static const conf_handler_node_t *_configuration_subnode_next(const conf_handler_node_t *handler,
                                                              char *key)
{
    const conf_handler_node_t *sub = handler->subnodes;
    while (sub) {
        if (_configuration_key_next(key, sub->subtree)) {
            return sub;
        }
        sub = container_of(sub->node.next, conf_handler_node_t, node);
    }
    return NULL;
}

static char *_configuration_key_array_index(char *key, long *index)
{
    char *endptr = key;
    long at = strtol(key, &endptr, 10);
    if (endptr == key) {
        return NULL;
    }
    else if (errno == ERANGE && ((at == LONG_MIN) || (at == LONG_MAX) || (at < 0))) {
        return NULL;
    }
    while (*endptr == '/') {
        endptr++;
    }
    *index = at;
    return endptr;
}

static int _configuration_find_handler(const conf_handler_node_t **next_handler, char **key, unsigned *offset)
{
    char *next = *key;
    const conf_handler_node_t *handler = *next_handler;
    if (!next || !handler ||
        !_configuration_key_next(*key, (*next_handler)->subtree)) {
        return -ENOENT;
    }
    long index;
    while ((next = _configuration_key_next(*key, (*next_handler)->subtree)) && *(*key = next)) {
        long off = -1;
        do {
            if ((handler = _configuration_subnode_next(*next_handler, next))) {
                if ((*next_handler)->ops &&
                    container_of(*next_handler, conf_handler_t, node)->handles_array &&
                    off < 0) {
                    /* for an array there must be an index before a new handler, like
                       if config was an array /config/next is invalid */
                    return -ENOENT;
                }
                *next_handler = handler;
            }
            else if ((next = _configuration_key_array_index(next, &index))) {
                *key = next;
                if (!(*next_handler)->ops ||
                    !container_of(*next_handler, conf_handler_t, node)->handles_array ||
                    off >= 0) {
                    /* if ther is an index, it must be an array */
                    /* do not allow multidimensional array, like config/0/1 */
                    return -ENOENT;
                }
                if ((unsigned)(off = index * container_of(*next_handler, conf_handler_t, node)->size)
                    >= container_of(*next_handler, conf_array_handler_t, handler.node)->array_size *
                       container_of(*next_handler, conf_handler_t, node)->size) {
                    /* index out of bounds */
                    return -ENOENT;
                }
                *offset += off;
            }
        } while (!handler && next);
    }
    return 0;
}

static void _configuration_iterator_init(conf_iterator_t *iter,
                                         const conf_handler_node_t *handler,
                                         bool max_depth)
{
    assert(iter);
    assert(handler);
    iter->root = handler;
    iter->sp = 0;
    iter->max_depth = max_depth;
    iter->stack[iter->sp++] = handler;
}

static void _configuration_append_next(conf_key_buf_t *key, const conf_handler_node_t *next)
{
    /* need to find l '/' for a node on level l in the tree */
    char *slash = key->buf;
    assert(*slash == '/');
    if (slash[strlen(slash) - 1] != '/') {
        strcat(key->buf, "/");
    }
    for (unsigned l = 0; l < next->level; slash++) {
        slash = strchr(slash, '/');
        assert(slash);
        if (!isdigit((int)slash[1])) {
            l++;
        }
    }
    strcpy(slash, next->subtree);
    strcat(key->buf, "/");
}

static void _configuration_remove_trailing(conf_key_buf_t *key)
{
    /*present key without trailing slash */
    if (key->buf[strlen(key->buf) - 1] == '/') {
        key->buf[strlen(key->buf) - 1] = '\0';
    }
}

static conf_handler_node_t *_configuration_handler_iterator_next(conf_iterator_t *iter, conf_key_buf_t *key)
{
    if (iter->sp > 0) {
        const conf_handler_node_t *next = iter->stack[--iter->sp];
        if (next != iter->root) {
            if (key) {
                _configuration_append_next(key, next);
            }
            if (next->node.next) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = container_of(next->node.next, conf_handler_node_t, node);
            }
        }
        const conf_handler_node_t *subnodes = next->subnodes;
        if (subnodes) {
            if (!next->ops || iter->max_depth) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = subnodes;
            }
        }
        if (key) {
            _configuration_remove_trailing(key);
        }
        return (conf_handler_node_t *)next;

    }
    return NULL;
}

static int _configuration_append_index(conf_key_buf_t *key, const conf_handler_node_t *next)
{
    assert(container_of(next, conf_handler_t, node)->handles_array);
    char *slash = &key->buf[strlen(key->buf) - 1];
    if (*slash != '/') {
        strcat(slash, "/");
        slash = strrchr(key->buf, '/');
    }
    uint32_t index = 0;
    if (!isdigit((int)slash[-1])) {
        strcat(slash, "0");
    }
    else {
        slash = strrchr(slash, '/');
        assert(slash);
        index = scn_u32_dec(slash + 1, strlen(key->buf) - strlen(slash));
        slash[fmt_u32_dec(slash + 1, ++index) + 1] = '\0';
    }
    return (index + 1) < container_of(next, conf_array_handler_t, handler.node)->array_size
        ? (index + 1) : 0;
}

static conf_handler_node_t *_configuration_path_iterator_next(conf_iterator_t *iter,
                                                              conf_key_buf_t *key)
{
    if (iter->sp > 0) {
        const conf_handler_node_t *next = iter->stack[--iter->sp];
        if (next != iter->root) {
            if (next->ops && container_of(next, conf_handler_t, node)->handles_array) {
                if (_configuration_append_index(key, next) > 0) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = next;
                }
                else if (next->node.next) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = container_of(next->node.next, conf_handler_node_t, node);
                }
            }
            else {
                _configuration_append_next(key, next);
                if (next->node.next) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = container_of(next->node.next, conf_handler_node_t, node);;
                }
            }
        }
        const conf_handler_node_t *subnodes = next->subnodes;
        if (subnodes && container_of(next, conf_handler_t, node)->conf_flags.export_as_a_whole == 0) {
            if (!next->ops || iter->max_depth) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = subnodes;
            }
        }
        _configuration_remove_trailing(key);
        return (conf_handler_node_t *)next;
    }
    return NULL;
}

static int _configuration_prepare(const conf_handler_node_t **next_handler, conf_key_buf_t *key)
{
    unsigned offset = 0;
    key->next = key->buf;
    if (_configuration_find_handler(next_handler, &key->next, &offset) < 0) {
        return -ENOENT;
    }
    key->offset = offset;
    int ret;
    if (*key->next) {
        if (&(key->next)[-1] == key->buf) {
            return -ENOENT;
        }
        assert((key->next)[-1] == '/');
        (key->next)[-1] = '\0';
        ret = strlen(key->buf);
    }
    else {
        ret = strlen(key->buf);
        if (key->buf[strlen(key->buf) - 1] != '/') {
            strcat(key->buf, "/");
        }
        /* if there is no dynamic next part, "next" and "key" should not overlap in memory */
        key->next = "";
    }
    return ret;
}

void configuration_key_restore(conf_key_buf_t *key_buf, const char *key, size_t key_len)
{
    memcpy(key_buf->buf, key, key_len);
    key_buf->buf[key_len] = '\0';
    if (*key_buf->next) {
        key_buf->buf[key_len] = '/';
        strcpy(&key_buf->buf[key_len + 1], key_buf->next);
    }
}

static int _configuration_handler_set_internal(const conf_handler_node_t *root,
                                               conf_key_buf_t *key, const void *value,
                                               size_t *size)
{
    assert(root);
    assert(key);
    assert((value && size && *size) || (!value && !size));

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, false);
    int ret = 0;
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops || !handler->ops->set) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        size_t sz = size ? *size : 0;
        if ((ret = handler->ops->set(container_of(handler, conf_handler_t, node), key, value, size))) {
            break;
        }
        if (value) {
            value = (const uint8_t *)value + (sz - *size);
        }
    }
    configuration_key_restore(key, key->buf, key_len);
    return ret;
}

static int _configuration_handler_get_internal(const conf_handler_node_t *root,
                                               conf_key_buf_t *key, void *value, size_t *size)
{
    assert(root);
    assert(key);
    assert(value);
    assert(size);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, false);
    int ret = 0;
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops || !handler->ops->get) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        size_t sz = *size;
        if ((ret = handler->ops->get(container_of(handler, conf_handler_t, node), key, value, size))) {
            break;
        }
        value = (uint8_t *)value + (sz - *size);
    }
    configuration_key_restore(key, key->buf, key_len);
    return ret;
}

static int _configuration_handler_import_internal(const conf_handler_node_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, true);
    conf_handler_node_t *handler;
    while ((handler = _configuration_path_iterator_next(&iter, key))) {
        if (!handler->ops || !handler->ops->import) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        handler->ops->import(container_of(handler, conf_handler_t, node), key);
    }
    configuration_key_restore(key, key->buf, key_len);
    return 0;
}

static int _configuration_handler_export_internal(const conf_handler_node_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, true);
    int ret = 0;
    conf_handler_node_t *handler;
    while ((handler = _configuration_path_iterator_next(&iter, key))) {
        if (!handler->ops || !handler->ops->export) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        if (handler->ops_dat && handler->ops_dat->verify) {
            if (handler->ops_dat->verify(container_of(handler, conf_handler_t, node), key)) {
                continue;
            }
        }
        if ((ret = handler->ops->export(container_of(handler, conf_handler_t, node), key))) {
            break;
        }
    }
    configuration_key_restore(key, key->buf, key_len);
    return ret;
}

static int _configuration_handler_delete_internal(const conf_handler_node_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, true);
    conf_handler_node_t *handler;
    while ((handler = _configuration_path_iterator_next(&iter, key))) {
        if (!handler->ops || !handler->ops->delete) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        handler->ops->delete(container_of(handler, conf_handler_t, node), key);
    }
    configuration_key_restore(key, key->buf, key_len);
    return 0;
}

static int _configuration_handler_apply_internal(const conf_handler_node_t *root,
                                                 conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, false);
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops_dat || !handler->ops_dat->apply) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        /* If this fails, there would be an inconsistency between verify() and applied values, or
           an API misuse where verify() was not called before. */
        handler->ops_dat->apply(container_of(handler, conf_handler_t, node), key);
    }
    configuration_key_restore(key, key->buf, key_len);
    return 0;
}

static int _configuration_handler_lock(const conf_handler_node_t *root, conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, true);
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops) {
            continue;
        }
        mutex_lock(&container_of(handler, conf_handler_t, node)->mutex);
    }
    configuration_key_restore(key, key->buf, key_len);
    return 0;
}

static int _configuration_handler_unlock(const conf_handler_node_t *root, conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, true);
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops) {
            continue;
        }
        mutex_unlock(&container_of(handler, conf_handler_t, node)->mutex);
    }
    configuration_key_restore(key, key->buf, key_len);
    return 0;
}

static int _configuration_handler_verify_internal(const conf_handler_node_t *root, conf_key_buf_t *key,
                                                  bool try_reimport)
{
    assert(root);
    assert(key);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, false);
    int ret = 0;
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops_dat || !handler->ops_dat->verify) {
            if (*key->next) {
                return -ENOENT;
            }
            continue;
        }
        if (handler->ops_dat->verify(container_of(handler, conf_handler_t, node), key)) {
            if (!try_reimport || !handler->ops->import) {
                ret = -ECANCELED;
                break;
            }
            else if ((ret = _configuration_handler_import_internal(configuration_get_root(), key))) {
                break;
            }
            if (handler->ops_dat->verify(container_of(handler, conf_handler_t, node), key)) {
                ret = -ECANCELED;
                break;
            }
        }
    }
    configuration_key_restore(key, key->buf, key_len);
    return ret;
}

static int _configuration_set_backend_internal(const conf_handler_node_t *root, conf_key_buf_t *key,
                                               const conf_backend_t *src_backend,
                                               const conf_backend_t *dst_backend)
{
    assert(root);
    assert(key);
    assert(src_backend);

    int key_len;
    if ((key_len = _configuration_prepare(&root, key)) <= 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    _configuration_iterator_init(&iter, root, false);
    int ret = 0;
    conf_handler_node_t *handler;
    while ((handler = _configuration_handler_iterator_next(&iter, key))) {
        if (!handler->ops) {
            continue;
        }
        container_of(handler, conf_handler_t, node)->src_backend = src_backend;
        container_of(handler, conf_handler_t, node)->dst_backend = dst_backend;
    }
    configuration_key_restore(key, key->buf, key_len);
    return ret;
}

conf_handler_node_t *configuration_get_root(void)
{
    return &_conf_root_handler;
}

void configuration_register(conf_handler_node_t *parent, conf_handler_node_t *node)
{
    assert(parent);
    assert(node);
    assert(!node->node.next); /* must be registered from the root to the leafs */
    conf_handler_node_t **end = &parent->subnodes;
    while (*end) {
        end = (conf_handler_node_t **)&(*end)->node.next;
    }
    node->level = parent->level + 1;
    *end = node;
}

int configuration_lock(conf_key_t *key)
{
    return _configuration_handler_lock(configuration_get_root(), key);
}

int configuration_unlock(conf_key_t *key)
{
    return _configuration_handler_unlock(configuration_get_root(), key);
}

int configuration_set(conf_key_t *key, const void *value, size_t *size)
{
    return _configuration_handler_set_internal(configuration_get_root(), key, value, size);
}

int configuration_verify(conf_key_t *key, bool try_reimport)
{
    return _configuration_handler_verify_internal(configuration_get_root(), key, try_reimport);
}

int configuration_get(conf_key_t *key, void *value, size_t *size)
{
    return _configuration_handler_get_internal(configuration_get_root(), key, value, size);
}

int configuration_import(conf_key_t *key)
{
    return _configuration_handler_import_internal(configuration_get_root(), key);
}

int configuration_export(conf_key_t *key)
{
    return _configuration_handler_export_internal(configuration_get_root(), key);
}

int configuration_delete(conf_key_t *key)
{
    return _configuration_handler_delete_internal(configuration_get_root(), key);
}

int configuration_apply(conf_key_t *key)
{
    return _configuration_handler_apply_internal(configuration_get_root(), key);
}

int configuration_set_backend(conf_key_t *key,
                              const conf_backend_t *src_backend,
                              const conf_backend_t *dst_backend)
{
    return _configuration_set_backend_internal(configuration_get_root(), key,
                                               src_backend, dst_backend);
}

char *configuration_key_prepend_root(conf_key_buf_t *key, const char *root)
{
    int root_len = strlen(root);
    int key_len = strlen(key->buf);
    int next_len = strlen(key->next);
    /* root | key \0 [ | next \0 ] */
    size_t len = root_len + key_len + 1 + (next_len ? next_len + 1 : 0);
    if (key->buf_len < len) {
        return NULL;
    }
    memmove(&key->buf[root_len], key->buf, len - root_len);
    memcpy(key->buf, root, root_len);
    if (next_len) {
        key->next = &key->buf[root_len + key_len + 1];
    }
    return &key->buf[root_len];
}
