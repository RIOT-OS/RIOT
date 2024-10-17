/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap_cache
 * @{
 *
 * @file
 * @brief       Implementation of common functions for the nanocoap-cache
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "kernel_defines.h"
#include "net/nanocoap/cache.h"
#include "hashes/sha256.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _cache_replacement_lru(void);
static int _cache_update_lru(clist_node_t *node);

static clist_node_t _cache_list_head = { NULL };
static clist_node_t _empty_list_head = { NULL };

static nanocoap_cache_entry_t _cache_entries[CONFIG_NANOCOAP_CACHE_ENTRIES];

static const nanocoap_cache_replacement_strategy_t _replacement_strategy = _cache_replacement_lru;
static const nanocoap_cache_update_strategy_t _update_strategy = _cache_update_lru;

static int _cache_replacement_lru(void)
{
    clist_node_t *lru_node = clist_lpeek(&_cache_list_head);

    /* no element in the list */
    if (!lru_node) {
        return -1;
    }

    nanocoap_cache_entry_t *lru_ce = container_of(lru_node, nanocoap_cache_entry_t, node);
    return nanocoap_cache_del(lru_ce);
}

static int _cache_update_lru(clist_node_t *node)
{
    if (clist_remove(&_cache_list_head, node)) {
        /* Move an accessed node to the end of the list. Least
         * recently used nodes are at the beginning of this list */
        clist_rpush(&_cache_list_head, node);
        return 0;
    }
    return -1;
}

void nanocoap_cache_init(void)
{
    _cache_list_head.next = NULL;
    _empty_list_head.next = NULL;
    memset(_cache_entries, 0, sizeof(_cache_entries));
    /* construct list of empty entries */
    for (unsigned i = 0; i < CONFIG_NANOCOAP_CACHE_ENTRIES; i++) {
        clist_rpush(&_empty_list_head, &_cache_entries[i].node);
    }
}

size_t nanocoap_cache_used_count(void)
{
    return clist_count(&_cache_list_head);
}

size_t nanocoap_cache_free_count(void)
{
    return clist_count(&_empty_list_head);
}

static void _cache_key_digest_opts(const coap_pkt_t *req, sha256_context_t *ctx,
        bool include_etag,
        bool include_blockwise)
{
    coap_optpos_t opt = {0, 0};
    uint8_t *value;

    for (unsigned i = 0; i < req->options_len; i++) {
        ssize_t optlen = coap_opt_get_next(req, &opt, &value, !i);
        if (optlen >= 0) {
            /* gCoAP forward proxy is ETag-aware, so skip ETag option,
             * see https://datatracker.ietf.org/doc/html/rfc7252#section-5.4.2 */
            if ((!include_etag) && (opt.opt_num == COAP_OPT_ETAG)) {
                continue;
            }
            /* skip NoCacheKey,
               see https://tools.ietf.org/html/rfc7252#section-5.4.6 */
            if ((opt.opt_num & 0x1E) == 0x1C) {
                continue;
            }
            /* Don't include blockwise (on request) so matching between
             * blockwise parts is possible */
            if ((!include_blockwise) && (
                    (opt.opt_num == COAP_OPT_BLOCK2) ||
                    (opt.opt_num == COAP_OPT_BLOCK1)
                    )) {
                continue;
            }
            sha256_update(ctx, &opt.opt_num, sizeof(opt.opt_num));
            sha256_update(ctx, value, optlen);
        }
    }
}

void nanocoap_cache_key_options_generate(const coap_pkt_t *req, void *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);
    _cache_key_digest_opts(req, &ctx, true, true);
    sha256_final(&ctx, cache_key);
}

void nanocoap_cache_key_blockreq_options_generate(const coap_pkt_t *req, void *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);
    _cache_key_digest_opts(req, &ctx, true, false);
    sha256_final(&ctx, cache_key);
}

void nanocoap_cache_key_generate(const coap_pkt_t *req, uint8_t *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);

    _cache_key_digest_opts(req, &ctx, !(IS_USED(MODULE_GCOAP_FORWARD_PROXY)), true);
    switch (coap_get_code_raw(req)) {
    case COAP_METHOD_FETCH:
        sha256_update(&ctx, req->payload, req->payload_len);
        break;
    default:
        break;
    }
    sha256_final(&ctx, cache_key);
}

ssize_t nanocoap_cache_key_compare(uint8_t *cache_key1, uint8_t *cache_key2)
{
    return memcmp(cache_key1, cache_key2, CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
}

static int _compare_cache_keys(clist_node_t *ce, void *arg)
{
    nanocoap_cache_entry_t *iterator = container_of(ce, nanocoap_cache_entry_t, node);
    uint8_t *cache_key = (uint8_t *) arg;

    if (!memcmp(iterator->cache_key, cache_key, CONFIG_NANOCOAP_CACHE_KEY_LENGTH)) {
        return 1;
    }
    return 0;
}

static clist_node_t *_nanocoap_cache_foreach(const uint8_t *key)
{
    return clist_foreach(&_cache_list_head, _compare_cache_keys, (uint8_t *)key);
}

nanocoap_cache_entry_t *nanocoap_cache_key_lookup(const uint8_t *key)
{
    clist_node_t *node = _nanocoap_cache_foreach(key);

    if (node) {
        _update_strategy(node);
    }

    return container_of(node, nanocoap_cache_entry_t, node);
}

nanocoap_cache_entry_t *nanocoap_cache_request_lookup(const coap_pkt_t *req)
{
    uint8_t cache_key[SHA256_DIGEST_LENGTH];
    nanocoap_cache_entry_t *ce;

    /* generate cache key */
    nanocoap_cache_key_generate(req, cache_key);

    /* check if cache key already exists */
    ce = nanocoap_cache_key_lookup(cache_key);

    return ce;
}

nanocoap_cache_entry_t *nanocoap_cache_process(const uint8_t *cache_key, unsigned request_method,
                                               const coap_pkt_t *resp, size_t resp_len)
{
    nanocoap_cache_entry_t *ce;
    ce = nanocoap_cache_key_lookup(cache_key);

    uint8_t code = coap_get_code_raw(resp);
    /* This response is not cacheable. */
    if (code == COAP_CODE_CREATED) {
        /* NO OP */
    }
    /* This response is not cacheable. However, a cache MUST mark any
       stored response for the deleted resource as not fresh.
    */
    else if (code == COAP_CODE_DELETED) {
        if (ce) {
            /* set max_age to now(), so that the cache is considered
             * stale immdiately */
            ce->max_age = ztimer_now(ZTIMER_SEC);
        }
    }
    /* When a cache that recognizes and processes the ETag response
       option receives a 2.03 (Valid) response, it MUST update the
       stored response with the value of the Max-Age Option included
       in the response (explicitly, or implicitly as a default value;
       see also Section 5.6.2). For each type of Safe-to-Forward
       option present in the response, the (possibly empty) set of
       options of this type that are present in the stored response
       MUST be replaced with the set of options of this type in the
       response received. (Unsafe options may trigger similar
       option-specific processing as defined by the option.)
    */
    else if (code == COAP_CODE_VALID) {
        if (ce) {
            /* refresh max_age() */
            uint32_t max_age = 60;
            coap_opt_get_uint((coap_pkt_t *)resp, COAP_OPT_MAX_AGE, &max_age);
            ce->max_age = ztimer_now(ZTIMER_SEC) + max_age;
        }
        /* TODO: handle the copying of the new options (if changed) */
    }
    /* This response is not cacheable. However, a cache MUST mark any
       stored response for the changed resource as not fresh.
    */
    else if (code == COAP_CODE_CHANGED) {
        if (ce) {
            /* set max_age to now(), so that the cache is considered
             * stale immdiately */
            ce->max_age = ztimer_now(ZTIMER_SEC);
        }
    }
    /* This response is cacheable: Caches can use the Max-Age Option
       to determine freshness and (if present) the
       ETag Option for validation.
    */
    else if (code == COAP_CODE_CONTENT) {
        if ((ce = nanocoap_cache_add_by_key(cache_key, request_method,
                                            resp, resp_len)) == NULL) {
            /* no space left in the cache? */
            return NULL;
        }
    }

    return ce;
}
static nanocoap_cache_entry_t *_nanocoap_cache_pop(void)
{
    clist_node_t *node;

    node = clist_lpop(&_empty_list_head);
    if (node != NULL) {
        return container_of(node, nanocoap_cache_entry_t, node);
    }
    else {
        return NULL;
    }
}

nanocoap_cache_entry_t *nanocoap_cache_add_by_key(const uint8_t *cache_key,
                                                  unsigned request_method,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len)
{
    nanocoap_cache_entry_t *ce = nanocoap_cache_key_lookup(cache_key);
    bool add_to_cache = false;

    if (resp_len > CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE) {
        DEBUG("nanocoap_cache: response too large to cache (%" PRIuSIZE "> %d)\n",
              resp_len, CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE);
        return NULL;
    }

    if (!ce) {
        /* did not find .. get an empty cache container */
        ce = _nanocoap_cache_pop();
        add_to_cache = true;
    }

    /* no space left */
    if (!ce) {
        /* could not remove any entry */
        if (_replacement_strategy()) {
            return NULL;
        }
        /* could remove an entry */
        ce = _nanocoap_cache_pop();
        add_to_cache = true;
        if (!ce) {
            /* still no free space ? stop trying now */
            return NULL;
        }
    }

    memcpy(ce->cache_key, cache_key, CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
    memcpy(&ce->response_pkt, resp, sizeof(coap_pkt_t));
    memcpy(&ce->response_buf, resp->buf, resp_len);
    ce->response_pkt.buf = ce->response_buf;
    ce->response_pkt.payload = ce->response_buf + (resp->payload - resp->buf);
    ce->response_len = resp_len;
    ce->request_method = request_method;

    /* default value is 60 seconds, if MAX_AGE not present */
    uint32_t max_age = 60;
    coap_opt_get_uint((coap_pkt_t *)resp, COAP_OPT_MAX_AGE, &max_age);
    ce->max_age = ztimer_now(ZTIMER_SEC) + max_age;

    if (add_to_cache) {
        clist_rpush(&_cache_list_head, &ce->node);
    }

    return ce;
}

nanocoap_cache_entry_t *nanocoap_cache_add_by_req(const coap_pkt_t *req,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len)
{
    uint8_t cache_key[SHA256_DIGEST_LENGTH];

    /* generate cache key */
    nanocoap_cache_key_generate(req, cache_key);

    return nanocoap_cache_add_by_key(cache_key,
                                     coap_get_code_raw((coap_pkt_t *)req),
                                     resp,
                                     resp_len);
}

int nanocoap_cache_del(const nanocoap_cache_entry_t *ce)
{
    clist_node_t *entry = clist_find(&_cache_list_head, &ce->node);

    if (entry) {
        clist_remove(&_cache_list_head, entry);
        memset(entry, 0, sizeof(nanocoap_cache_entry_t));
        clist_rpush(&_empty_list_head, entry);
        return 0;
    }

    return -1;
}
