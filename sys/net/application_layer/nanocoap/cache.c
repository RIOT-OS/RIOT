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

#include "net/nanocoap/cache.h"
#include "hashes/sha256.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static clist_node_t _cache_list_head = { NULL };
static clist_node_t _empty_list_head = { NULL };

static nanocoap_cache_entry_t _cache_entries[CONFIG_NANOCOAP_CACHE_ENTRIES];

static nanocoap_cache_replacement_strategy_t _replacement_strategy = NULL;

static int _cache_replacement_lru(void)
{
    nanocoap_cache_entry_t *least = NULL;
    clist_node_t *it = _cache_list_head.next;

    /* no element in the list */
    if (!it) {
        return -1;
    }

    least = (nanocoap_cache_entry_t *) it;

    do {
        it = it->next;
        nanocoap_cache_entry_t *nit = (nanocoap_cache_entry_t *) it;
        if (least->access_time > nit->access_time) {
            least = nit;
        }
    } while (it != _cache_list_head.next);

    return nanocoap_cache_del(least);
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

    /* hardcode cache replacement strategy for now */
    _replacement_strategy = _cache_replacement_lru;
}

size_t nanocoap_cache_used_count(void)
{
    return clist_count(&_cache_list_head);
}

size_t nanocoap_cache_free_count(void)
{
    return clist_count(&_empty_list_head);
}

void nanocoap_cache_key_generate(const coap_pkt_t *req, uint8_t *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);

    coap_optpos_t opt = {0, 0};
    uint8_t *value;

    for (int i = 0; i < req->options_len; i++) {
        ssize_t optlen = coap_opt_get_next(req, &opt, &value, !i);
        if (optlen >= 0) {
            /* skip NoCacheKey,
               see https://tools.ietf.org/html/rfc7252#section-5.4.6 */
            if ((opt.opt_num & 0x1E) == 0x1C) {
                continue;
            }
            sha256_update(&ctx, value, optlen);
        }
    }
    sha256_final(&ctx, cache_key);
}

ssize_t nanocoap_cache_key_compare(uint8_t *cache_key1, uint8_t *cache_key2)
{
    return memcmp(cache_key1, cache_key2, CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
}

static int _compare_cache_keys(clist_node_t *ce, void *arg)
{
    nanocoap_cache_entry_t *iterator = (nanocoap_cache_entry_t *) ce;
    uint8_t *cache_key = (uint8_t *) arg;

    if (!memcmp(iterator->cache_key, cache_key, CONFIG_NANOCOAP_CACHE_KEY_LENGTH)) {
        return 1;
    }
    return 0;
}

nanocoap_cache_entry_t *nanocoap_cache_key_lookup(const uint8_t *key)
{
    nanocoap_cache_entry_t *ce;

    ce = (nanocoap_cache_entry_t *) clist_foreach(&_cache_list_head,
                                                  _compare_cache_keys,
                                                  (uint8_t *)key);

    if (ce) {
        ce->access_time = xtimer_now_usec64();
    }

    return ce;
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

int nanocoap_cache_process(const uint8_t *cache_key, unsigned request_method,
                           const coap_pkt_t *resp, size_t resp_len)
{
    nanocoap_cache_entry_t *ce;
    ce = nanocoap_cache_key_lookup(cache_key);

    /* This response is not cacheable. */
    if (resp->hdr->code == COAP_CODE_CREATED) {
        /* NO OP */
    }
    /* This response is not cacheable. However, a cache MUST mark any
       stored response for the deleted resource as not fresh.
    */
    else if (resp->hdr->code == COAP_CODE_DELETED) {
        if (ce) {
            /* set max_age to now(), so that the cache is considered
             * stale immdiately */
            ce->max_age = xtimer_now_usec64();
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
    else if (resp->hdr->code == COAP_CODE_VALID) {
        if (ce) {
            /* refresh max_age() */
            uint32_t max_age = 60;
            coap_opt_get_uint(resp, COAP_OPT_MAX_AGE, &max_age);
            ce->max_age = xtimer_now_usec64() + (max_age * US_PER_SEC);
        }
        /* TODO: handle the copying of the new options (if changed) */
    }
    /* This response is not cacheable. However, a cache MUST mark any
       stored response for the changed resource as not fresh.
    */
    else if (resp->hdr->code == COAP_CODE_CHANGED) {
        if (ce) {
            /* set max_age to now(), so that the cache is considered
             * stale immdiately */
            ce->max_age = xtimer_now_usec64();
        }
    }
    /* This response is cacheable: Caches can use the Max-Age Option
       to determine freshness and (if present) the
       ETag Option for validation.
    */
    else if (resp->hdr->code == COAP_CODE_CONTENT) {
        uint64_t now = xtimer_now_usec64();
        /* cache entry is stale */
        if (ce && (ce->max_age < now)) {
            nanocoap_cache_del(ce);
        }
        if (NULL == nanocoap_cache_add_by_key(cache_key, request_method,
                                              resp, resp_len)) {
            /* no space left in the cache? */
            return -1;
        }
        /* TODO: ETAG handling */
    }

    return 0;
}

nanocoap_cache_entry_t *nanocoap_cache_add_by_key(const uint8_t *cache_key,
                                                  unsigned request_method,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len)
{
    nanocoap_cache_entry_t *ce;
    ce = nanocoap_cache_key_lookup(cache_key);

    /* found an already existing cache entry */
    if (ce) {
        return ce;
    }

    /* did not find .. get an empty cache container */
    ce = (nanocoap_cache_entry_t *) clist_lpop(&_empty_list_head);

    /* no space left */
    if (!ce) {
        /* could not remove any entry */
        if (_replacement_strategy()) {
            return NULL;
        }
        /* could remove an entry */
        ce = (nanocoap_cache_entry_t *) clist_lpop(&_empty_list_head);
        if (!ce) {
            /* still no free space ? stop trying now */
            return NULL;
        }
    }

    memcpy(ce->cache_key, cache_key, CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
    memcpy(&ce->response_pkt, resp, sizeof(coap_pkt_t));
    memcpy(&ce->response_buf, resp->hdr, resp_len);
    ce->response_pkt.hdr = (coap_hdr_t *) ce->response_buf;
    ce->response_len = resp_len;
    ce->access_time = xtimer_now_usec64();
    ce->request_method = request_method;

    /* default value is 60 seconds, if MAX_AGE not present */
    uint32_t max_age = 60;
    coap_opt_get_uint(resp, COAP_OPT_MAX_AGE, &max_age);
    ce->max_age = xtimer_now_usec64() + (max_age * US_PER_SEC);

    clist_rpush(&_cache_list_head, &ce->node);

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
                                     coap_get_code((coap_pkt_t *)req),
                                     resp,
                                     resp_len);
}

int nanocoap_cache_del(const nanocoap_cache_entry_t *ce)
{
    clist_node_t *entry = clist_find(&_cache_list_head, &ce->node);

    if (entry) {
        clist_remove(&_cache_list_head, entry);
        memset((nanocoap_cache_entry_t *) entry, 0,
               sizeof(nanocoap_cache_entry_t));
        clist_rpush(&_empty_list_head, entry);
        return 0;
    }

    return -1;
}
