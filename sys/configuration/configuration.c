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
#include "configuration_iterator.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _HANDLER(n)             container_of((n), conf_handler_t, node)
#define _ARRAY_HANDLER(n)       container_of((n), conf_array_handler_t, handler)

#if IS_USED(MODULE_CONFIGURATION_STRINGS)
#define _KEY_BUF(k) ((k)->buf)
#define _KEY_BUF_LEN(k) ((k)->buf_len)
#else
#define _KEY_BUF(k) NULL
#define _KEY_BUF_LEN(k) 0
#endif

int configuration_set_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, const void *val,
                                      size_t *size);

int configuration_get_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, void *val,
                                      size_t *size);

int configuration_import_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

int configuration_export_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

int configuration_delete_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

int configuration_encode_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key, const conf_sid_t *sid_start,
                                         conf_fmt_t fmt, void **enc_data, size_t *enc_size);

int configuration_decode_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key, conf_sid_t *sid_start,
                                         conf_fmt_t fmt, const void **dec_data, size_t *dec_size);

#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
const conf_handler_backend_ops_t configuration_default_backend_ops = {
    .import = configuration_import_handler_default,
    .export = configuration_export_handler_default,
    .delete = configuration_delete_handler_default,
};

const conf_handler_backend_data_ops_t configuration_default_backend_data_ops = {
    .encode = configuration_encode_handler_default,
    .decode = configuration_decode_handler_default,
};
#endif

static CONF_HANDLER_NODE_ID(_conf_root_handler_node_id, 0, UINT64_MAX, "");

static CONF_HANDLER(_conf_root_handler,
                    &_conf_root_handler_node_id,
                    NULL,
                    &configuration_default_backend_ops,
                    &configuration_default_backend_data_ops,
                    0, NULL);

static conf_iterator_restore_t _configuration_iterator_init(conf_iterator_t *iter,
                                                            const conf_handler_t *handler,
                                                            const conf_key_buf_t *key,
                                                            char *key_buf)
{
    (void)key_buf;
    assert(iter);
    assert(handler);
    iter->root = handler;
    iter->sp = 0;
    iter->stack[iter->sp++] = (conf_iterator_item_t) {
        handler
    };
    return (conf_iterator_restore_t) { .sid = key->sid
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
                                     , .key_len = key_buf ? strlen(key_buf) : 0
#endif
    };
}

static conf_iterator_restore_t _configuration_path_iterator_init(conf_path_iterator_t *iter,
                                                                 const conf_handler_t *handler,
                                                                 const conf_key_buf_t *key,
                                                                 char *key_buf)
{
    (void)key_buf;
    assert(iter);
    assert(handler);
    iter->root = handler;
    iter->sp = 0;
    iter->stack[iter->sp++] = (conf_path_iterator_item_t) {
        handler,
        key->sid > handler->node_id->sid_lower
            ? _sid_array_index(_ARRAY_HANDLER(handler), key->sid) : 0
    };
    return (conf_iterator_restore_t) { .sid = key->sid
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
                                      , .key_len = key_buf ? strlen(key_buf) : 0
#endif
    };
}

static bool _sid_in_node_range(const conf_handler_t *handler, conf_sid_t sid)
{
    return handler->node_id->sid_lower <= sid && sid <= handler->node_id->sid_upper;
}

static bool _sid_in_array_range(const conf_array_handler_t *array, conf_sid_t sid)
{
    return array->handler.array_id->sid_lower <= sid && sid <= array->handler.array_id->sid_upper;
}

static bool _sid_in_range(const conf_handler_t *handler, conf_sid_t sid)
{
    if (handler->conf_flags.handles_array) {
        return _sid_in_array_range(_ARRAY_HANDLER(handler), sid);
    }
    else if (!handler->conf_flags.handles_primitive) {
        return _sid_in_node_range(handler, sid);
    }
    return handler->handler_id->sid_lower == sid;
}

static bool _sid_in_array_bounds(const conf_array_handler_t *array, conf_sid_t sid)
{
    assert(sid > array->handler.array_id->sid_lower);
    return _sid_array_index(array, sid) < array->array_size;
}

static int _configuration_append_segment(const conf_handler_t *next, conf_key_buf_t *key,
                                         char *key_buf, size_t key_buf_len)
{
    (void)next; (void)key; (void)key_buf; (void)key_buf_len;
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    if (key_buf) {
        char *buf = key_buf;
        size_t size = key_buf_len;
        if (*next->node_id->subtree) {
            for (int l = 0; l < (int)next->level - 1; l++) {
                if (*buf++ != '/') {
                    return -EINVAL;
                }
                char *slash;
                if ((slash = strchr(buf, '/'))) {
                    if (isdigit((int)slash[1])) {
                        if ((slash = strchr(++slash, '/'))) {
                            buf = slash;
                        }
                    }
                    else {
                        buf = slash;
                    }
                }
            }
            if (*buf == '/') {
                *buf = '\0';
            }
            assert(size > strlen(key_buf) + 1 + strlen(next->node_id->subtree) + 1);
            strcat(buf, "/");
            strcat(buf, next->node_id->subtree);
        }
    }
#endif
    return 0;
}

static void _debug_print(const conf_key_buf_t *key)
{
    (void)key;
#if ENABLE_DEBUG
    char ssid[17];
    ssid[fmt_u64_hex(ssid, key->sid)] = '\0';
    DEBUG("configuration: %16s %10u %s\n", ssid, key->offset,
          configuration_key_buf((conf_key_t *)key));
#endif
}

static int _configuration_append_index(uint32_t index, conf_key_buf_t *key,
                                       char *key_buf, size_t key_buf_len)
{
    (void)index; (void)key; (void)key_buf; (void)key_buf_len;
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    if (key_buf) {
        char *buf = key_buf;
        size_t size = key_buf_len;
        char sindex[11];
        sindex[fmt_u32_dec(sindex, index)] = '\0';
        assert(size > strlen(buf) + 1 + strlen(sindex) + 1);
        strcat(buf, "/");
        strcat(buf, sindex);
    }
#endif
    return 0;
}

static int _configuration_find_handler_sid(const conf_handler_t **next_handler,
                                           conf_key_buf_t *key, char *key_buf, size_t key_buf_len)
{
    assert(next_handler);
    assert(key);

    const conf_handler_t *handler = *next_handler;

    if (!handler || !_sid_in_range(handler, key->sid_normal)) {
        return -ENOENT;
    }
    while (handler) {
        if (key->sid_normal > handler->node_id->sid_lower) {
            if (handler->conf_flags.handles_array) {
                if (!_sid_in_array_bounds(_ARRAY_HANDLER(handler), key->sid_normal)) {
                    return -ERANGE;
                }
                uint32_t index = _sid_array_index(_ARRAY_HANDLER(handler), key->sid_normal);
                /* Set data offset */
                key->offset += index * handler->size;
                /* map to first array element (normalize) */
                key->sid_normal -= (index * handler->array_id->sid_stride);

                if (_configuration_append_index(index, key, key_buf, key_buf_len)) {
                    return -ENOBUFS;
                }
            }
        }
        for (handler = handler->subnodes; handler; handler = _HANDLER(handler->node.next)) {
            if (!_sid_in_range(handler, key->sid_normal)) {
                continue;
            }
            if (_configuration_append_segment(handler, key, key_buf, key_buf_len)) {
                return -ENOBUFS;
            }
             *next_handler = handler;
            break;
        }
    }
    if (key->sid_normal != (*next_handler)->node_id->sid_lower) {
        if ((*next_handler)->conf_flags.handles_array) {
            if (!_sid_in_array_bounds(_ARRAY_HANDLER(*next_handler), key->sid_normal)) {
                return -ERANGE;
            }
        }
        else {
            return -ENOENT;
        }
    }
    _debug_print(key);
    return 0;
}


static int _configuration_prepare_sid(const conf_handler_t **next_handler, conf_key_buf_t *key,
                                      char *key_buf, size_t key_buf_len)
{
    key->offset = 0;
    key->sid_normal = key->sid;
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    if (key_buf) {
        memset(key_buf, 0, key_buf_len);
    }
#endif
    int ret = _configuration_find_handler_sid(next_handler, key, key_buf, key_buf_len);
    if (ret < 0) {
        (void)ret;
        DEBUG("configuration: no handler found %d, SID %llu\n", ret, key->sid);
    }
    return ret;
}

static conf_handler_t *_configuration_handler_sid_iterator_next(conf_iterator_t *iter, conf_key_buf_t *key)
{
    if (iter->sp > 0) {
        conf_iterator_item_t next = iter->stack[--iter->sp];
        if (next.node != iter->root) {
            if (_configuration_append_segment(next.node, key, _KEY_BUF(key), _KEY_BUF_LEN(key))) {
                return NULL;
            }
            key->sid += next.node->node_id->sid_lower - key->sid_normal;
            key->sid_normal = next.node->node_id->sid_lower;
            if (next.node->node.next) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_iterator_item_t) { _HANDLER(next.node->node.next) };
            }
        }
        if (next.node->subnodes) {
            assert(iter->sp < ARRAY_SIZE(iter->stack));
            iter->stack[iter->sp++] = (conf_iterator_item_t) { next.node->subnodes };
        }
        _debug_print(key);
        return (conf_handler_t *)next.node;
    }
    return NULL;
}

static conf_handler_t *_configuration_handler_encode_iterator_next(conf_path_iterator_t *iter,
                                                                   conf_key_buf_t *key,
                                                                   const conf_sid_t *sid_start)
{
    if (iter->sp > 0) {
        conf_path_iterator_item_t next = iter->stack[--iter->sp];
        if (next.node->node_id->sid_lower > key->sid) {
            key->sid = next.node->node_id->sid_lower;
            key->sid_normal = next.node->node_id->sid_lower;
            key->offset = 0;
        }
        else if (next.node->node_id->sid_lower > key->sid_normal) {
            key->sid += next.node->node_id->sid_lower - key->sid_normal;
            key->sid_normal = next.node->node_id->sid_lower;
        }
        bool push_subnodes = false;
        if (next.node->conf_flags.handles_array && next.index < _ARRAY_HANDLER(next.node)->array_size) {
            assert(iter->sp < ARRAY_SIZE(iter->stack));
            iter->stack[iter->sp++] = (conf_path_iterator_item_t) { next.node, next.index + 1 };
            if (key->sid != *sid_start && key->sid_normal != next.node->node_id->sid_lower) {
                if (next.node->node_id->sid_lower + 1 + next.index * next.node->array_id->sid_stride > key->sid) {
                    key->sid = next.node->node_id->sid_lower + 1 + next.index * next.node->array_id->sid_stride;
                    key->offset = next.index * next.node->size;
                }
                else {
                    key->sid += (next.node->array_id->sid_stride - (key->sid_normal - (next.node->array_id->sid_lower + 1)));
                    key->offset += next.node->size;
                }
                key->sid_normal = next.node->node_id->sid_lower + 1;
            }
            push_subnodes = true;
        }
        else if (!next.node->conf_flags.handles_primitive && next.index < 1) {
            assert(iter->sp < ARRAY_SIZE(iter->stack));
            iter->stack[iter->sp++] = (conf_path_iterator_item_t) { next.node, 1 };
            if (key->sid != *sid_start) {
                key->sid += next.node->node_id->sid_lower - key->sid_normal;
                key->sid_normal = next.node->node_id->sid_lower;
            }
            push_subnodes = true;
        }
        if (!push_subnodes && key->sid != *sid_start) {
            if (next.node->node.next) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_path_iterator_item_t) { _HANDLER(next.node->node.next), 0 };
            }
        }
        else if (push_subnodes) {
            if (next.node->subnodes) {
                /* Stop encoding when the backend changes, because subnode is exported to another backend */
                const conf_backend_t *be = *configuration_get_dst_backend(iter->root);
                const conf_backend_t *be_h = *configuration_get_dst_backend(next.node);
                if (!be_h || be == be_h) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = (conf_path_iterator_item_t) { next.node->subnodes, 0 };
                }
            }
        }
        return (conf_handler_t *)next.node;
    }
    return NULL;
}

static conf_handler_t *_configuration_handler_decode_iterator_next(conf_path_iterator_t *iter,
                                                                   conf_key_buf_t *key,
                                                                   const conf_handler_t *handler,
                                                                   const conf_sid_t *sid_start)
{
    if (iter->sp > 0) {
        conf_path_iterator_item_t next = iter->stack[--iter->sp];
        if (handler) {
            /* got a new key: go deeper by finding the next handler */
            if (!next.node->conf_flags.handles_primitive) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_path_iterator_item_t) { next.node, next.index};
            }
            if (next.node != handler && !handler->conf_flags.handles_primitive) {
                uint32_t index = 0;
                if (handler->conf_flags.handles_array && key->sid > handler->node_id->sid_lower) {
                    index = _sid_array_index(_ARRAY_HANDLER(handler), key->sid);
                }
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_path_iterator_item_t) { handler, index + 1};
            }
        }
        else {
            handler = next.node;
            if (handler == iter->root && key->sid == *sid_start) {
                /* push back root if nothing changed */
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_path_iterator_item_t) { handler, next.index + 1};
            }
            else {
                /* no new key: go upper by finishing an element */
                if (handler->conf_flags.handles_array && next.index < _ARRAY_HANDLER(handler)->array_size) {
                    if (handler->array_id->sid_lower + 1 + next.index * handler->array_id->sid_stride > key->sid) {
                        key->sid = handler->array_id->sid_lower + 1 + next.index * handler->array_id->sid_stride;
                        key->offset = next.index * handler->size;
                    }
                    else {
                        key->sid += (handler->array_id->sid_stride - (key->sid_normal - (handler->array_id->sid_lower + 1)));
                        key->offset += handler->size;
                    }
                    key->sid_normal = handler->node_id->sid_lower + 1;
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = (conf_path_iterator_item_t) { handler, next.index + 1 };
                }
                else {
                    key->sid = handler->node_id->sid_upper;
                    key->sid_normal = key->sid;
                }
            }
        }
        return (conf_handler_t *)handler;
    }
    return NULL;
}

static conf_handler_t *_configuration_path_sid_iterator_next(conf_path_iterator_t *iter,
                                                             conf_key_buf_t *key,
                                                             const conf_sid_t *sid_start)
{
    if (iter->sp > 0) {
        conf_path_iterator_item_t next = iter->stack[--iter->sp];
        if (next.node->node_id->sid_lower > key->sid) {
            key->sid = next.node->node_id->sid_lower;
            key->sid_normal = next.node->node_id->sid_lower;
            key->offset = 0;
        }
        else if (next.node->node_id->sid_lower > key->sid_normal) {
            key->sid += next.node->node_id->sid_lower - key->sid_normal;
            key->sid_normal = next.node->node_id->sid_lower;
        }
        if (key->sid != *sid_start && _configuration_append_segment(next.node, key, _KEY_BUF(key), _KEY_BUF_LEN(key))) {
            return NULL;
        }
        if (next.node->conf_flags.handles_array) {
            bool skip = key->sid == *sid_start && next.node->node_id->sid_lower < key->sid_normal;
            if (next.index == 0) {
                if (key->sid_normal == next.node->node_id->sid_lower) {
                    key->sid++;
                    (key->sid_normal)++;
                }
            }
            else if (key->sid != *sid_start) {
                if (next.node->node_id->sid_lower + 1 + next.index * next.node->array_id->sid_stride > key->sid) {
                    key->sid = next.node->node_id->sid_lower + 1 + next.index * next.node->array_id->sid_stride;
                    key->offset = next.index * next.node->size;
                }
                else {
                    key->sid += (next.node->array_id->sid_stride - (key->sid_normal - (next.node->array_id->sid_lower + 1)));
                    key->offset += next.node->size;
                }
                key->sid_normal = next.node->node_id->sid_lower + 1;
            }
            if (key->sid != *sid_start && _configuration_append_index(next.index, key, _KEY_BUF(key), _KEY_BUF_LEN(key))) {
                return NULL;
            }
            if (!skip) {
                if (++next.index < _ARRAY_HANDLER(next.node)->array_size) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = next;
                }
                else if (next.node != iter->root && next.node->node.next) {
                    assert(iter->sp < ARRAY_SIZE(iter->stack));
                    iter->stack[iter->sp++] = (conf_path_iterator_item_t) { _HANDLER(next.node->node.next), 0 };
                }
            }
        }
        else {
            key->sid_normal = next.node->node_id->sid_lower;
            if (next.node != iter->root && next.node->node.next) {
                assert(iter->sp < ARRAY_SIZE(iter->stack));
                iter->stack[iter->sp++] = (conf_path_iterator_item_t) { _HANDLER(next.node->node.next), 0 };
            }
        }
        if (next.node->subnodes) {
            assert(iter->sp < ARRAY_SIZE(iter->stack));
            iter->stack[iter->sp++] = (conf_path_iterator_item_t) { next.node->subnodes, 0 };
        }
        _debug_print(key);
        return (conf_handler_t *)next.node;
    }
    return NULL;
}

static void _configuration_key_restore(conf_key_buf_t *key, const conf_iterator_restore_t *restore,
                                       char *key_buf)
{
    (void)key_buf;
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    if (key_buf) {
        key_buf[restore->key_len] = '\0';
    }
#endif
    key->sid = restore->sid;
}

static int _configuration_handler_set_internal(const conf_handler_t *root,
                                               conf_key_buf_t *key, const void *value,
                                               size_t *size)
{
    assert(root);
    assert(key);
    assert((value && size && *size) || (!value && !size));

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_handler_t *handler = (conf_handler_t *)root;
    if (!handler->size || !handler->data) {
        return -ENOTSUP;
    }
    return configuration_set_handler_default(handler, key, value, size);
}

static int _configuration_handler_get_internal(const conf_handler_t *root,
                                               conf_key_buf_t *key, void *value, size_t *size)
{
    assert(root);
    assert(key);
    assert(value);
    assert(size);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_handler_t *handler = (conf_handler_t *)root;
    if (!handler->size || !handler->data) {
        return -ENOTSUP;
    }
    return configuration_get_handler_default(handler, key, value, size);
}

static int _configuration_handler_import_internal(const conf_handler_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_path_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_path_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_path_sid_iterator_next(&iter, key, &restore.sid))) {
        const conf_backend_t *be = *configuration_get_src_backend(handler);
        if (!be || !be->ops || !be->ops->be_load) {
            continue;
        }
        if (handler != root && be == *configuration_get_src_backend(root)) {
            continue;
        }
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        if (!handler->ops_be || !handler->ops_be->import) {
            continue;
        }
        handler->ops_be->import(handler, key);
#else
        configuration_import_handler_default(handler, key);
#endif
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return 0;
}

static int _configuration_handler_export_internal(const conf_handler_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    int ret = 0;
    conf_path_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_path_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_path_sid_iterator_next(&iter, key, &restore.sid))) {
        const conf_backend_t *be = *configuration_get_dst_backend(handler);
        if (!be || !be->ops || !be->ops->be_store) {
            continue;
        }
        if (handler != root && be == *configuration_get_dst_backend(root)) {
            continue;
        }
        if (handler->ops_dat && handler->ops_dat->verify) {
            if (handler->ops_dat->verify(handler, key)) {
                continue;
            }
        }
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        if (!handler->ops_be || !handler->ops_be->export) {
            continue;
        }
        ret = handler->ops_be->export(handler, key);
#else
        ret = configuration_export_handler_default(handler, key);
#endif
        if (ret < 0) {
            break;
        }
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return ret;
}

static int _configuration_handler_delete_internal(const conf_handler_t *root,
                                                  conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_path_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_path_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_path_sid_iterator_next(&iter, key, &restore.sid))) {
        const conf_backend_t *be = *configuration_get_src_backend(handler);
        if (!be || !be->ops || !be->ops->be_delete) {
            continue;
        }
        if (handler != root && be == *configuration_get_src_backend(root)) {
            continue;
        }
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        if (!handler->ops_be || !handler->ops_be->delete) {
            continue;
        }
        handler->ops_be->delete(handler, key);
#else
        configuration_delete_handler_default(handler, key);
#endif
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return 0;
}

static int _configuration_handler_apply_internal(const conf_handler_t *root,
                                                 conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_handler_sid_iterator_next(&iter, key))) {
        if (!handler->ops_dat || !handler->ops_dat->apply) {
            continue;
        }
        /* If this fails, there would be an inconsistency between verify() and applied values, or
           an API misuse where verify() was not called before. */
        handler->ops_dat->apply(handler, key);
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return 0;
}

static int _configuration_handler_lock(const conf_handler_t *root, conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_handler_sid_iterator_next(&iter, key))) {
        mutex_lock(&handler->mutex);
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return 0;
}

static int _configuration_handler_unlock(const conf_handler_t *root, conf_key_buf_t *key)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    conf_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_handler_sid_iterator_next(&iter, key))) {
        mutex_unlock(&handler->mutex);
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return 0;
}

static int _configuration_handler_verify_internal(const conf_handler_t *root, conf_key_buf_t *key,
                                                  bool try_reimport)
{
    assert(root);
    assert(key);

    if (_configuration_prepare_sid(&root, key, _KEY_BUF(key), _KEY_BUF_LEN(key)) < 0) {
        return -ENOENT;
    }
    int ret = 0;
    conf_iterator_t iter;
    conf_iterator_restore_t restore = _configuration_iterator_init(&iter, root, key, _KEY_BUF(key));
    conf_handler_t *handler;
    while ((handler = _configuration_handler_sid_iterator_next(&iter, key))) {
        if (!handler->ops_dat || !handler->ops_dat->verify) {
            continue;
        }
        if (handler->ops_dat->verify(handler, key)) {
            if (!try_reimport) {
                ret = -ECANCELED;
                break;
            }
            else if ((ret = configuration_import(key))) {
                break;
            }
            if (handler->ops_dat->verify(handler, key)) {
                ret = -ECANCELED;
                break;
            }
        }
    }
    _configuration_key_restore(key, &restore, _KEY_BUF(key));
    return ret;
}

conf_handler_t *configuration_get_root(void)
{
    return &_conf_root_handler;
}

void configuration_register(conf_handler_t *parent, conf_handler_t *node)
{
    assert(parent);
    assert(node);
    assert(!node->node.next); /* must be registered from the root to the leafs */
    conf_handler_t **end = &parent->subnodes;
    while (*end) {
        end = (conf_handler_t **)&(*end)->node.next;
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

int configuration_encode_internal(conf_path_iterator_t *iter, conf_iterator_restore_t *restore,
                                  const conf_handler_t **root, conf_key_buf_t *key,
                                  void *buf, size_t *size)
{
    assert(iter);
    assert(restore);
    assert(root);
    assert(key);
    assert(buf);
    assert(size);

    int ret = 0;
    if (!iter->root) {
        *restore = _configuration_path_iterator_init(iter, *root, key, NULL);
    }
    else {
        /* continue where encoding failed previously */
        assert(*root);
        const conf_backend_t *be = *configuration_get_dst_backend(iter->root);
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        assert((*root)->ops_be_dat);
        assert((*root)->ops_be_dat->encode);
        ret = (*root)->ops_be_dat->encode(*root, key, &restore->sid, be->fmt, &buf, size);
#else
        ret = configuration_encode_handler_default(*root, key, &restore->sid, be->fmt, &buf, size);
#endif
        if (ret) {
            return -ECANCELED;
        }
    }
    conf_handler_t *handler;
    while ((handler = _configuration_handler_encode_iterator_next(iter, key, &restore->sid))) {
        const conf_backend_t *be = *configuration_get_dst_backend(iter->root);
        /* do not export a subnode which has an own backend set
           because this would export the same data to two backends */
        const conf_backend_t *be_h = *configuration_get_dst_backend(handler);
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        if (!handler->ops_be_dat || !handler->ops_be_dat->encode) {
            continue;
        }
        if (be_h && be_h != be && be_h->ops && be_h->ops->be_store) {
            continue;
        }
        ret = handler->ops_be_dat->encode(handler, key, &restore->sid, be->fmt, &buf, size);
#else
        if (be_h && be_h != be && be_h->ops && be_h->ops->be_store) {
            continue;
        }
        ret = configuration_encode_handler_default(handler, key, &restore->sid, be->fmt, &buf, size);
#endif
        if (ret) {
            /* save node to continue with, after encoding failure */
            *root = handler;
            break;
        }
    }
    return ret;
}

int configuration_decode_internal(conf_path_iterator_t *iter, conf_iterator_restore_t *restore,
                                  const conf_handler_t **root, conf_key_buf_t *key,
                                  const void *buf, size_t *size)
{
    assert(iter);
    assert(restore);
    assert(root);
    assert(key);
    assert(buf);
    assert(size);

    int ret = 0;
    const conf_handler_t *handler = NULL;
    if (!iter->root) {
        *restore = _configuration_path_iterator_init(iter, *root, key, NULL);
        *root = NULL;
    }
    do {
        size_t size_cpy = *size;
        /* decode new SID */
        const conf_handler_t *next = configuration_get_root();
        const conf_backend_t *be = *configuration_get_src_backend(iter->root);
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
        ret = iter->root->ops_be_dat->decode(iter->root, NULL, &key->sid, be->fmt, &buf, &size_cpy);
#else
        ret = configuration_decode_handler_default(iter->root, NULL, &key->sid, be->fmt, &buf, &size_cpy);
#endif
        bool skip = false; /* skip structure is parsed SID is not known */
        if (!ret) {
            key->offset = 0;
            key->sid_normal = key->sid;
            /* for a new SID we must start from the root to get the full key->offset */
            if ((ret = _configuration_prepare_sid(&next, key, NULL, 0)) < 0) {
                if (ret == -ENOENT) {
                    skip = true;
                }
                else {
                    return ret;
                }
            }
        }
        else {
            if (ret == -ENOBUFS) {
                break;
            }
            next = NULL;
        }
        /* get handler of SID */
        if ((skip && (handler = iter->root)) ||
            (handler = *root) ||
            (handler = *root = _configuration_handler_decode_iterator_next(iter, key, next, &restore->sid))) {
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS)
            if (!handler->ops_be_dat || !handler->ops_be_dat->decode) {
                continue;
            }
            ret = handler->ops_be_dat->decode(skip ? NULL : handler, key, &restore->sid, be->fmt, &buf, &size_cpy);
#else
            ret = configuration_decode_handler_default(skip ? NULL : handler, key, &restore->sid, be->fmt, &buf, &size_cpy);
#endif
            if (ret) {
                break;
            }
            *root = NULL;
        }
        *size = size_cpy;
    } while (handler && *size);

    return ret;
}
