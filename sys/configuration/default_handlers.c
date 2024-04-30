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
 * @brief       Implementation of runtime configuration default handlers
 *
 * These handler implementations can be used if no dynamic key part is used.
 * This is the case if every path segment in the key has a node in the configuration tree.
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

#include "macros/utils.h"
#include "modules.h"
#include "container.h"
#include "configuration.h"
#include "configuration_iterator.h"

#if IS_USED(MODULE_NANOCBOR)
#include <nanocbor/nanocbor.h>
#endif

#ifndef PRIu64
#define PRIu64 "llu"
#endif

#define ENABLE_DEBUG    0
#include "debug.h"

#define _ARRAY_HANDLER(h)       container_of((h), conf_array_handler_t, handler)

static mutex_t _enc_mutex = MUTEX_INIT; /* encode buffer needs concurrency protection */
static mutex_t _dec_mutex = MUTEX_INIT; /* decode buffer needs concurrency protection */
static uint8_t _enc_buffer[CONFIGURATION_ENCODING_BUF_SIZE];
static uint8_t _dec_buffer[CONFIGURATION_DECODING_BUF_SIZE];

int configuration_encode_internal(conf_path_iterator_t *iter, conf_iterator_restore_t *restore,
                                  const conf_handler_t **root, conf_key_buf_t *key,
                                  void *buf, size_t *size);

int configuration_decode_internal(conf_path_iterator_t *iter, conf_iterator_restore_t *restore,
                                  const conf_handler_t **root, conf_key_buf_t *key,
                                  const void *buf, size_t *size);

int configuration_set_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, const void *val,
                                      size_t *size)
{
    assert(handler);
    assert(key);
    assert(handler->size);
    assert(handler->data);
    assert((val && size && *size) || (!val && !size));

    size_t sz = size ? *size : 0;
    if (handler->conf_flags.handles_array && handler->array_id->sid_lower == key->sid_normal) {
        if (val && sz < handler->size * _ARRAY_HANDLER(handler)->array_size) {
            return -ENOBUFS;
        }
        sz = handler->size * _ARRAY_HANDLER(handler)->array_size;
    }
    else {
        if (val && sz < handler->size) {
            return -ENOBUFS;
        }
        sz = handler->size;
    }
    uint8_t *data = (uint8_t *)handler->data + key->offset;
    if (val) {
        memcpy(data, val, sz);
        *size -= sz;
    }
    else {
        memset(data, 0, sz);
    }
    return 0;
}

int configuration_get_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, void *val,
                                      size_t *size)
{
    assert(handler);
    assert(val);
    assert(key);
    assert(size);
    assert(handler->size);
    assert(handler->data);

    size_t sz = handler->size;
    if (handler->conf_flags.handles_array && handler->array_id->sid_lower == key->sid_normal) {
        if (*size < (sz = handler->size * _ARRAY_HANDLER(handler)->array_size)) {
            return -ENOBUFS;
        }
    }
    const uint8_t *data = (const uint8_t *)handler->data + key->offset;
    *size -= sz;
    memcpy(val, data, sz);
    return 0;
}

int configuration_import_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key)
{
    assert(handler);
    assert(key);
    assert(*configuration_get_src_backend(handler));
    assert((*configuration_get_src_backend(handler))->ops);
    assert((*configuration_get_src_backend(handler))->ops->be_load);

    conf_sid_t sid_restore = key->sid;
    size_t sz = handler->size;
    const conf_backend_t *be = *configuration_get_src_backend(handler);
    if (handler->conf_flags.handles_array && handler->array_id->sid_lower == key->sid_normal) {
        sz = handler->size * _ARRAY_HANDLER(handler)->array_size;
    }
    int err = 0; (void)err;
    if (be->fmt != CONF_FMT_RAW) {
        size_t total = 0;
        conf_path_iterator_t iter = { .root = NULL };
        conf_iterator_restore_t restore;
        const conf_handler_t *dec_handler = handler;
        conf_key_buf_t dec_key = *key;
        conf_backend_flags_t flg = 0;
        size_t full_dec_size = sizeof(_dec_buffer);
        mutex_lock(&_dec_mutex);
        do {
            /* if decoding failed with ENOBUFS before it is not allowed to fail again */
            bool abort_on_failure = (err < 0);
            if (total + sizeof(_dec_buffer) >= full_dec_size) {
                flg |= CONF_BACKEND_FLAG_FINISH;
            }
            void *dec_data = _dec_buffer;
            size_t dec_size = MIN(sizeof(_dec_buffer), full_dec_size - total);
            if ((err = be->ops->be_load(be, key, dec_data, &dec_size, total, &flg))) {
                DEBUG("configuration: backend importing key %s failed (%d)\n",
                        configuration_key_buf(key), err);
                goto restore_key;
            }
            if (!total) {
                /* first read returns the full size */
                full_dec_size = dec_size;
            }
            if (flg & CONF_BACKEND_FLAG_MORE) {
                dec_size = sizeof(_dec_buffer);
                flg &= ~CONF_BACKEND_FLAG_MORE;
            }
            sz = dec_size;
            if (!(err = configuration_decode_internal(&iter, &restore, &dec_handler, &dec_key, dec_data, &dec_size)) ||
                (err == -ENOBUFS && !abort_on_failure)) {
                total += sz - dec_size;
            }
            else {
                DEBUG("configuration: decoding for key %s failed (%d)\n",
                      configuration_key_buf(key), err);
                goto restore_key;
            }
        } while (total < full_dec_size);
    }
    else if (handler->data) {
        uint8_t *data = (uint8_t *)handler->data + key->offset;
        conf_backend_flags_t flg = CONF_BACKEND_FLAG_FINISH;
        if ((err = be->ops->be_load(be, key, data, &sz, 0, &flg))) {
            DEBUG("configuration: backend importing key %s failed (%d)\n",
                configuration_key_buf(key), err);
        }
    }

restore_key:
    mutex_unlock(&_dec_mutex);
    key->sid = sid_restore;
    return 0;
}

static bool _check_subkey(const conf_key_buf_t *key)
{
    (void)key;
#if IS_USED(MODULE_CONFIGURATION_KEY_SUBKEY)
    return !key->subkey || key->sid > key->subkey->sid_lower;
#endif
    return true;
}

int configuration_export_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key)
{
    assert(handler);
    assert(key);
    assert(*configuration_get_dst_backend(handler));
    assert((*configuration_get_dst_backend(handler))->ops);
    assert((*configuration_get_dst_backend(handler))->ops->be_store);

    conf_sid_t sid_restore = key->sid;
    size_t sz = handler->size;
    const conf_backend_t *be = *configuration_get_dst_backend(handler);
    const uint8_t *data = handler->data ? (const uint8_t *)handler->data + key->offset : NULL;
    if (handler->conf_flags.handles_array && handler->array_id->sid_lower == key->sid_normal) {
        sz = handler->size * _ARRAY_HANDLER(handler)->array_size;
    }
    int err; (void)err;
    size_t total = 0;
    conf_key_buf_t enc_key = *key;
    conf_backend_flags_t flg = 0;
    if (be->fmt != CONF_FMT_RAW) {
        conf_path_iterator_t iter = { .root = NULL };
        conf_iterator_restore_t restore;
        const conf_handler_t *enc_handler = handler;
        void *enc_data = _enc_buffer;
        size_t enc_size = sizeof(_enc_buffer);
        mutex_lock(&_enc_mutex);
        while ((err = configuration_encode_internal(&iter, &restore, &enc_handler, &enc_key, enc_data, &enc_size)) != 0) {
            if (err == -ENOBUFS) {
                /* need to flush */
                sz = sizeof(_enc_buffer) - enc_size;
                if (_check_subkey(&enc_key)) {
                    if ((err = be->ops->be_store(be, key, enc_data, &sz, total, &flg))) {
                        DEBUG("configuration: backend exporting key %s failed (%d)\n",
                                configuration_key_buf(key), err);
                    }
                }
                total += sz;
                enc_data = _enc_buffer;
                enc_size = sizeof(_enc_buffer);
            }
            else {
                DEBUG("configuration: encoding for key %s failed (%d)\n",
                      configuration_key_buf(key), err);
                goto restore_key;
            }
        }
        data = enc_data;
        sz = sizeof(_enc_buffer) - enc_size;
    }
    if (data) {
        flg |= CONF_BACKEND_FLAG_FINISH;
        if ((err = be->ops->be_store(be, key, data, &sz, total, &flg))) {
            DEBUG("configuration: backend exporting key %s failed (%d)\n",
                configuration_key_buf(key), err);
        }
    }

restore_key:
    mutex_unlock(&_enc_mutex);
    key->sid = sid_restore;
    return 0;
}

int configuration_delete_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key)
{
    assert(handler);
    assert(key);
    assert(*configuration_get_src_backend(handler));
    assert((*configuration_get_src_backend(handler))->ops);

    const conf_backend_t *be = *configuration_get_src_backend(handler);
    if (!be->ops->be_delete) {
        return -ENOTSUP;
    }
    int err; (void)err;
    if ((err = be->ops->be_delete(be, key))) {
        DEBUG("configuration: backend deleting key %s failed (%d)\n",
                configuration_key_buf(key), err);
    }

    return 0;
}

static int _encode_node_handler_cbor(const conf_handler_t *handler,
                                     conf_key_buf_t *key, const conf_sid_t *sid_start,
                                     void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    if (key->sid_normal == handler->node_id->sid_lower) {
        if (*sid_start != key->sid) {
            uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
            sid = key->sid - handler->parent->node_id->sid_lower;
#endif
            if (nanocbor_fmt_uint(&enc, sid) < 0) {
                DEBUG("configuration: failed to encode node SID %"PRIu64" handler %p\n",
                      key->sid, (const void *)handler);
                return -ENOBUFS;
            }
        }
        if (nanocbor_fmt_map_indefinite(&enc) < 0) {
            DEBUG("configuration: failed to encode map for SID %"PRIu64" handler %p\n",
                  key->sid, (const void *)handler);
            return -ENOBUFS;
        }
    }
    else {
        if (nanocbor_fmt_end_indefinite(&enc) < 0) {
            DEBUG("configuration: failed to encode map end for SID %"PRIu64" handler %p\n",
                  key->sid, (const void *)handler);
            return -ENOBUFS;
        }
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_node_handler_cbor(const conf_handler_t *handler,
                                     conf_key_buf_t *key, conf_sid_t *sid_start,
                                     const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    if (!key) {
        uint64_t sid;
        if ((ret = nanocbor_get_uint64(&dec, &sid)) < 0) {
            DEBUG("configuration: failed to decode SID handler %p\n",
                  (const void *)handler);
            return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
        }
        *sid_start = sid;
    }
    else {
        if (key->sid_normal == handler->node_id->sid_lower) {
            nanocbor_value_t map;
            if ((ret = nanocbor_enter_map(&dec, &map)) < 0) {
                DEBUG("configuration: failed to decode map for SID %"PRIu64" handler %p\n",
                      key->sid, (const void *)handler );
                return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
            }
            dec = map;
        }
        else {
            /* disobey nanoCBOR API to use the same map object */
            nanocbor_value_t map = dec;
            if (dec.end > dec.cur && *dec.cur == 0xFF) {
                map.flags = NANOCBOR_DECODER_FLAG_CONTAINER | NANOCBOR_DECODER_FLAG_INDEFINITE;
                nanocbor_leave_container(&dec, &map);
            }
        }
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _encode_array_handler_cbor(const conf_handler_t *handler,
                                      conf_key_buf_t *key, const conf_sid_t *sid_start,
                                      void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    const conf_array_handler_t *arr_handler = _ARRAY_HANDLER(handler);
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    if (key->sid_normal == arr_handler->handler.array_id->sid_lower) {
        if (*sid_start != key->sid) {
            uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
            sid = key->sid - handler->parent->array_id->sid_lower;
#endif
            if (nanocbor_fmt_uint(&enc, sid) < 0) {
                DEBUG("configuration: failed to encode array SID %"PRIu64" handler %p\n",
                      key->sid, (const void *)arr_handler);
                return -ENOBUFS;
            }
        }
        if (nanocbor_fmt_array(&enc, arr_handler->array_size) < 0) {
            DEBUG("failed to encode array for SID %"PRIu64" handler %p\n",
                  key->sid, (const void *)arr_handler);
            return -ENOBUFS;
        }
        if (nanocbor_fmt_map_indefinite(&enc) < 0) {
            DEBUG("configuration: failed to encode array [0] for SID %"PRIu64" handler %p\n",
                  key->sid, (const void *)arr_handler);
            return -ENOBUFS;
        }
    }
    else if (!_sid_array_remainder(arr_handler, key->sid)) {
        if (key->sid != *sid_start) {
            /* finish map of previous call if the key SID is not the start SID */
            if (nanocbor_fmt_end_indefinite(&enc) < 0) {
                DEBUG("configuration: failed to encode array map end for SID %"PRIu64" handler %p\n",
                      key->sid, (const void *)arr_handler);
                return -ENOBUFS;
            }
        }
        if (nanocbor_fmt_map_indefinite(&enc) < 0) {
            DEBUG("configuration: failed to encode array [%"PRIu32"] for SID %"PRIu64" handler %p\n",
                  _sid_array_index(arr_handler, key->sid), key->sid, (const void *)arr_handler);
            return -ENOBUFS;
        }
    }
    else {
        /* close last map */
        if (nanocbor_fmt_end_indefinite(&enc) < 0) {
            DEBUG("configuration: failed to encode array map end for SID %"PRIu64" handler %p\n",
                  key->sid, (const void *)arr_handler);
            return -ENOBUFS;
        }
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_array_handler_cbor(const conf_handler_t *handler,
                                      conf_key_buf_t *key, conf_sid_t *sid_start,
                                      const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    const conf_array_handler_t *arr_handler = _ARRAY_HANDLER(handler);
    nanocbor_value_t dec;
    int ret;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    if (!key) {
        uint64_t sid;
        if ((ret = nanocbor_get_uint64(&dec, &sid)) < 0) {
            DEBUG("configuration: failed to decode SID handler %p\n",
                  (const void *)handler);
            return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
        }
        *sid_start = sid;
    }
    else {
        if (key->sid_normal == arr_handler->handler.array_id->sid_lower) {
            nanocbor_value_t array;
            nanocbor_value_t map;
            if ((ret = nanocbor_enter_array(&dec, &array)) < 0) {
                DEBUG("configuration: failed to decode array for SID %"PRIu64" handler %p\n",
                    key->sid, (const void *)arr_handler);
                return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
            }
            if ((ret = nanocbor_enter_map(&array, &map)) < 0) {
                DEBUG("configuration: failed to decode array [0] for SID %"PRIu64" handler %p\n",
                    key->sid, (const void *)arr_handler);
                return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
            }
            dec = map;
        }
        else if (!_sid_array_remainder(arr_handler, key->sid)) {
            nanocbor_value_t map = dec;
            if (key->sid != *sid_start) {
                /* close last opened map */
                if (key->sid != *sid_start) {
                    if (dec.end > dec.cur && *dec.cur == 0xFF) {
                        map.flags = NANOCBOR_DECODER_FLAG_CONTAINER | NANOCBOR_DECODER_FLAG_INDEFINITE;
                        nanocbor_leave_container(&dec, &map);
                    }
                }
            }
            if ((ret = nanocbor_enter_map(&dec, &map)) < 0) {
                DEBUG("configuration: failed to decode array [%"PRIu32"] for SID %"PRIu64" handler %p\n",
                    _sid_array_index(arr_handler, key->sid), key->sid, (const void *)arr_handler);
                return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
            }
            dec = map;
        }
        else {
            /* close last opened map */
            nanocbor_value_t map = dec;
            if (dec.end > dec.cur && *dec.cur == 0xFF) {
                map.flags = NANOCBOR_DECODER_FLAG_CONTAINER | NANOCBOR_DECODER_FLAG_INDEFINITE;
                nanocbor_leave_container(&dec, &map);
            }
        }
    }
    *dec_size -= (dec.cur - (const uint8_t *)*dec_data);
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _encode_uint_cbor(const conf_handler_t *handler,
                             conf_key_buf_t *key, const conf_sid_t *sid_start,
                             void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    const void *data = ((const uint8_t *)handler->data) + key->offset;
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
    sid = key->sid - handler->parent->node_id->sid_lower;
#endif
    if (nanocbor_fmt_uint(&enc, sid) < 0) {
        DEBUG("configuration: failed to encode SID %"PRIu64" for CBOR uint handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    uint64_t uint_val = 0;
    switch (handler->size) {
        case 1:
            uint_val = *((const uint8_t *)data);
            break;
        case 2:
            uint_val = *((const uint16_t *)data);
            break;
        case 4:
            uint_val = *((const uint32_t *)data);
            break;
        case 8:
            uint_val = *((const uint64_t *)data);
            break;
        default:
            assert(false);
            break;
    }
    if (nanocbor_fmt_uint(&enc, uint_val) < 0) {
        DEBUG("configuration: failed to encode CBOR uint for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_uint_cbor(const conf_handler_t *handler,
                             conf_key_buf_t *key, conf_sid_t *sid_start,
                             const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret = NANOCBOR_NOT_FOUND;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    union {
        uint8_t uint8_val; uint16_t uint16_val; uint32_t uint32_val; uint64_t uint64_val;
    } uint_val = { 0 };
    unsigned uint_size;
    switch ((uint_size = handler->size)) {
        case 1:
            ret = nanocbor_get_uint8(&dec, &uint_val.uint8_val);
            break;
        case 2:
            ret = nanocbor_get_uint16(&dec, &uint_val.uint16_val);
            break;
        case 4:
            ret = nanocbor_get_uint32(&dec, &uint_val.uint32_val);
            break;
        case 8:
            ret = nanocbor_get_uint64(&dec, &uint_val.uint64_val);
            break;
        default:
            assert(false);
            uint_size = 0;
            break;
    }
    if (ret < 0) {
        DEBUG("configuration: failed to decode uint for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
    }
    else {
        void *data = ((uint8_t *)handler->data) + key->offset;
        memcpy(data, &uint_val, uint_size);
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _encode_int_cbor(const conf_handler_t *handler,
                            conf_key_buf_t *key, const conf_sid_t *sid_start,
                            void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    const void *data = ((const uint8_t *)handler->data) + key->offset;
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
    sid = key->sid - handler->parent->node_id->sid_lower;
#endif
    if (nanocbor_fmt_uint(&enc, sid) < 0) {
        DEBUG("configuration: failed to encode SID %"PRIu64" for CBOR int handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    int64_t int_val = 0;
    switch (handler->size) {
        case 1:
            int_val = *((const int8_t *)data);
            break;
        case 2:
            int_val = *((const int16_t *)data);
            break;
        case 4:
            int_val = *((const int32_t *)data);
            break;
        case 8:
            int_val = *((const int64_t *)data);
            break;
        default:
            assert(false);
    }
    if (nanocbor_fmt_int(&enc, int_val) != NANOCBOR_OK) {
        DEBUG("configuration: failed to encode CBOR int for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}


static int _decode_int_cbor(const conf_handler_t *handler,
                            conf_key_buf_t *key, conf_sid_t *sid_start,
                            const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret = NANOCBOR_NOT_FOUND;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    union {
        int8_t int8_val; int16_t int16_val; int32_t int32_val; int64_t int64_val;
    } int_val = { 0 };
    unsigned int_size;
    switch ((int_size = handler->size)) {
        case 1:
            ret = nanocbor_get_int8(&dec, &int_val.int8_val);
            break;
        case 2:
            ret = nanocbor_get_int16(&dec, &int_val.int16_val);
            break;
        case 4:
            ret = nanocbor_get_int32(&dec, &int_val.int32_val);
            break;
        case 8:
            ret = nanocbor_get_int64(&dec, &int_val.int64_val);
            break;
        default:
            assert(false);
            int_size = 0;
            break;
    }
    if (ret < 0) {
        DEBUG("configuration: failed to decode int for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
    }
    else {
        void *data = ((uint8_t *)handler->data) + key->offset;
        memcpy(data, &int_val, int_size);
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _encode_byte_string_cbor(const conf_handler_t *handler,
                                    conf_key_buf_t *key, const conf_sid_t *sid_start,
                                    void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    const uint8_t *data = ((const uint8_t *)handler->data) + key->offset;
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
    sid = key->sid - handler->parent->node_id->sid_lower;
#endif
    if (nanocbor_fmt_uint(&enc, sid) < 0) {
        DEBUG("configuration: failed to encode SID %"PRIu64" for CBOR byte string handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    if (nanocbor_put_bstr(&enc, data, handler->size) != NANOCBOR_OK) {
        DEBUG("configuration: failed to encode CBOR byte string for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_byte_string_cbor(const conf_handler_t *handler,
                                    conf_key_buf_t *key, conf_sid_t *sid_start,
                                    const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    const uint8_t *bstr;
    size_t bstr_len;
    if ((ret = nanocbor_get_bstr(&dec, &bstr, &bstr_len)) < 0) {
        DEBUG("configuration: failed to decode byte string for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
    }
    if (handler->size < bstr_len) {
        DEBUG("configuration: byte string too large for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
    }
    else {
        void *data = ((uint8_t *)handler->data) + key->offset;
        memcpy(data, bstr, bstr_len);
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _encode_text_string_cbor(const conf_handler_t *handler,
                                    conf_key_buf_t *key, const conf_sid_t *sid_start,
                                    void **enc_data, size_t *enc_size)
{
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;
#if IS_USED(MODULE_NANOCBOR)
    const char *data = ((const char *)handler->data) + key->offset;
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, *enc_data, *enc_size);
    uint64_t sid = key->sid;
#if IS_USED(MODULE_CONFIGURATION_DELTA_ENCODING)
    sid = key->sid - handler->parent->node_id->sid_lower;
#endif
    if (nanocbor_fmt_uint(&enc, sid) < 0) {
        DEBUG("configuration: failed to encode SID %"PRIu64" for CBOR test string handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    if (nanocbor_put_tstr(&enc, data) != NANOCBOR_OK) {
        DEBUG("configuration: failed to encode CBOR text string for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return -ENOBUFS;
    }
    *enc_data = enc.cur;
    *enc_size -= nanocbor_encoded_len(&enc);
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_text_string_cbor(const conf_handler_t *handler,
                                    conf_key_buf_t *key, conf_sid_t *sid_start,
                                    const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    const uint8_t *tstr;
    size_t tstr_len;
    if ((ret = nanocbor_get_tstr(&dec, &tstr, &tstr_len)) < 0) {
        DEBUG("configuration: failed to decode text string for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
    }
    if (handler->size < tstr_len + 1) {
        DEBUG("configuration: text string too large for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
    }
    else {
        void *data = ((uint8_t *)handler->data) + key->offset;
        strncpy(data, (const char *)tstr, tstr_len);
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

static int _decode_skip_cbor(const conf_handler_t *handler,
                             conf_key_buf_t *key, conf_sid_t *sid_start,
                             const void **dec_data, size_t *dec_size)
{
    (void)handler; (void)key; (void)sid_start; (void)dec_data; (void)dec_size;
#if IS_USED(MODULE_NANOCBOR)
    nanocbor_value_t dec;
    int ret;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    if ((ret = nanocbor_skip(&dec)) < 0) {
        DEBUG("configuration: failed to skip CBOR data for SID %"PRIu64" handler %p\n",
              key->sid, (const void *)handler);
        return ret == NANOCBOR_ERR_END ? -ENOBUFS : -EINVAL;
    }
    *dec_size -= dec.cur - (const uint8_t *)*dec_data;
    *dec_data = dec.cur;
    return 0;
#endif
    return -ENOTSUP;
}

int configuration_encode_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key, const conf_sid_t *sid_start,
                                         conf_fmt_t fmt, void **enc_data, size_t *enc_size)
{
    assert(handler);
    assert(key);
    assert(sid_start);
    assert(enc_data);
    assert(enc_size);
    (void)handler; (void)key; (void)sid_start; (void)enc_data; (void)enc_size;

    if (fmt == CONF_FMT_CBOR) {
        if (IS_USED(MODULE_NANOCBOR)) {
            if (handler->conf_flags.handles_array) {
                return _encode_array_handler_cbor(handler, key, sid_start, enc_data, enc_size);
            }
            else if (handler->conf_flags.handles_primitive) {
                if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_UINT) {
                    return _encode_uint_cbor(handler, key, sid_start, enc_data, enc_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_INT) {
                    return _encode_int_cbor(handler, key, sid_start, enc_data, enc_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_BSTR) {
                    return _encode_byte_string_cbor(handler, key, sid_start, enc_data, enc_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_TSTR) {
                    return _encode_text_string_cbor(handler, key, sid_start, enc_data, enc_size);
                }
                else {
                    assert(false);
                }
            }
            return _encode_node_handler_cbor(handler, key, sid_start, enc_data, enc_size);
        }
    }
    return -ENOTSUP;
}

int configuration_decode_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key, conf_sid_t *sid_start,
                                         conf_fmt_t fmt, const void **dec_data, size_t *dec_size)
{
    assert(sid_start);
    assert(dec_data);
    assert(dec_size);
    (void)key; (void)sid_start; (void)dec_data; (void)dec_size;

    if (fmt == CONF_FMT_CBOR) {
        if (IS_USED(MODULE_NANOCBOR)) {
            if (!handler) {
                return _decode_skip_cbor(handler, key, sid_start, dec_data, dec_size);
            }
            else if (handler->conf_flags.handles_array) {
                return _decode_array_handler_cbor(handler, key, sid_start, dec_data, dec_size);
            }
            else if (handler->conf_flags.handles_primitive) {
                if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_UINT) {
                    return _decode_uint_cbor(handler, key, sid_start, dec_data, dec_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_INT) {
                    return _decode_int_cbor(handler, key, sid_start, dec_data, dec_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_BSTR) {
                    return _decode_byte_string_cbor(handler, key, sid_start, dec_data, dec_size);
                }
                else if (handler->conf_flags.primitive_type == CONF_PRIM_TYPE_TSTR) {
                    return _decode_text_string_cbor(handler, key, sid_start, dec_data, dec_size);
                }
                else {
                    assert(false);
                }
            }
            return _decode_node_handler_cbor(handler, key, sid_start, dec_data, dec_size);
        }
    }
    return -ENOTSUP;
}
