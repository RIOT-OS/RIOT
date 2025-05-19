/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_nanocoap_cache nanoCoAP-Cache implementation
 * @ingroup     net_nanocoap
 * @brief       A cache implementation for nanocoap response messages
 *
 * @{
 *
 * @file
 * @brief       nanocoap-cache API
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */


#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "clist.h"
#include "net/nanocoap.h"
#include "hashes/sha256.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The number of maximum cache entries.
 */
#ifndef CONFIG_NANOCOAP_CACHE_ENTRIES
#define CONFIG_NANOCOAP_CACHE_ENTRIES          (8)
#endif

/**
 * @brief The length of the cache key in bytes.
 */
#ifndef CONFIG_NANOCOAP_CACHE_KEY_LENGTH
#define CONFIG_NANOCOAP_CACHE_KEY_LENGTH       (8)
#endif

/**
 * @brief Size of the buffer to store responses in the cache.
 */
#ifndef CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE
#define CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE    (128)
#endif

/**
 * @brief   Cache container that holds a @p coap_pkt_t struct.
 */
typedef struct {
    /**
     * @brief needed for clist_t, must be the first struct member!
     */
    clist_node_t node;

    /**
     * @brief the calculated cache key, see nanocoap_cache_key_generate().
     */
    uint8_t cache_key[CONFIG_NANOCOAP_CACHE_KEY_LENGTH];

    /**
     * @brief packet representation of the response
     */
    coap_pkt_t response_pkt;

    /**
     * @brief buffer to hold the response message.
     */
    uint8_t response_buf[CONFIG_NANOCOAP_CACHE_RESPONSE_SIZE];

    size_t response_len; /**< length of the message in @p response */

    uint8_t request_method; /**< the method of the initial request */
#if IS_USED(MODULE_GCOAP) || defined(DOXYGEN)
    bool truncated;         /**< the cached response is truncated */
#endif  /* IS_USED(MODULE_GCOAP) || defined(DOXYGEN) */

    /**
     * @brief absolute system time in seconds until which this cache entry
     * is considered valid.
     */
    uint32_t max_age;
} nanocoap_cache_entry_t;

/**
 * @brief Typedef for the cache replacement strategy on full cache list.
 *
 * @return   0 on successfully replacing a cache element
 * @return  -1 on error
 */
typedef int (*nanocoap_cache_replacement_strategy_t)(void);

/**
 * @brief Typedef for the cache update strategy on element access.
 *
 * @param[in] node       The accessed node.
 *
 * @return   0 on successfully updating the element
 * @return  -1 on error
 */
typedef int (*nanocoap_cache_update_strategy_t)(clist_node_t *node);

/**
 * @brief   Initializes the internal state of the nanocoap cache.
 */
#if IS_USED(MODULE_NANOCOAP_CACHE)
void nanocoap_cache_init(void);
#else
static inline void nanocoap_cache_init(void)
{
    return;
}
#endif

/**
 * @brief   Returns the number of cached entries.
 *
 * @return  Number of cached entries
 */
size_t nanocoap_cache_used_count(void);

/**
 * @brief   Returns the number of unused cache entries.
 *
 * @return  Number of unused cache entries
 */
size_t nanocoap_cache_free_count(void);

/**
 * @brief   Determines if a response is cacheable and modifies the cache
 *          as reflected in RFC7252, Section 5.9.

 * @param[in] cache_key       The cache key of the request
 * @param[in] request_method  The method of the initial request
 * @param[in] resp            The response to operate on
 * @param[in] resp_len        The actual length of the response in @p resp
 *
 * @return  The cache entry on successfully handling the response
 * @return  NULL on error
 */
nanocoap_cache_entry_t *nanocoap_cache_process(const uint8_t *cache_key, unsigned request_method,
                                               const coap_pkt_t *resp, size_t resp_len);
/**
 * @brief   Creates a new or gets an existing cache entry using the
 *          request packet.
 *
 * @param[in] req           The request to calculate the cache-key
 * @param[in] resp          The response to add to the cache
 * @param[in] resp_len      The actual length of the response message in @p resp
 *
 * @return  The previously existing or newly added cache entry on success
 * @return  NULL, if there is no space left
 */
nanocoap_cache_entry_t *nanocoap_cache_add_by_req(const coap_pkt_t *req,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len);

/**
 * @brief   Creates a new or gets an existing cache entry using the cache key.
 *
 * @param[in] cache_key       The cache key of the request
 * @param[in] request_method  The method of the initial request
 * @param[in] resp            The response to add to the cache
 * @param[in] resp_len        The actual length of the response in @p resp
 *
 * @return  The previously existing or newly added cache entry on success
 * @return  NULL, if there is no space left
 */
nanocoap_cache_entry_t *nanocoap_cache_add_by_key(const uint8_t *cache_key,
                                                  unsigned request_method,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len);

/**
 * @brief   Performs a cache lookup based on the @p req.
 *
 * @param[in] req           The request to calculate the cache-key
 *
 * @return  An existing cache entry on cache hit
 * @return  NULL on cache miss
 */
nanocoap_cache_entry_t *nanocoap_cache_request_lookup(const coap_pkt_t *req);

/**
 * @brief   Performs a cache lookup based on the cache key of a request.
 *
 * @param[in] cache_key       The cache key of a request
 *
 * @return  An existing cache entry on cache hit
 * @return  NULL on cache miss
 */
nanocoap_cache_entry_t *nanocoap_cache_key_lookup(const uint8_t *cache_key);

/**
 * @brief   Deletes the provided cache entry @p ce.
 *
 * @param[in] ce            The cache entry to delete
 *
 * @return  0 on success
 * @return  -1 if entry is not available in the cache
 */
int nanocoap_cache_del(const nanocoap_cache_entry_t *ce);

/**
 * @brief   Generates a cache key based on the request @p req.
 *
 * @param[in] req           The request to generate the cache key from
 * @param[out] cache_key    The generated cache key
 */
void nanocoap_cache_key_generate(const coap_pkt_t *req, uint8_t *cache_key);

/**
 * @brief   Generates a cache key based on only the options in @p req
 *
 * @param[in] req           The request to generate the cache key from
 * @param[out] cache_key    The generated cache key of SHA256_DIGEST_LENGTH bytes
 */
void nanocoap_cache_key_options_generate(const coap_pkt_t *req, void *cache_key);

/**
 * @brief   Generates a cache key based on only the options in @p req without
 * any of the blockwise options included in the key
 *
 * This function can be used to correlate individual requests that are part of a
 * blockwise transfer with each other.
 *
 * @param[in] req           The request to generate the cache key from
 * @param[out] cache_key    The generated cache key of SHA256_DIGEST_LENGTH bytes
 */
void nanocoap_cache_key_blockreq_options_generate(const coap_pkt_t *req, void *cache_key);

/**
 * @brief   Compares two cache keys.
 *
 * @param[in] cache_key1    The first cache key in the comparison
 * @param[in] cache_key2    The second cache key in the comparison
 *
 * @return  0 if cache keys are equal
 * @return  <0 or 0> (see memcmp()) for unequal cache keys
 */
ssize_t nanocoap_cache_key_compare(uint8_t *cache_key1, uint8_t *cache_key2);

/**
 * @brief   Check if the Max-Age of a cache entry has passed
 *
 * @param[in] ce    A cache entry
 * @param[in] now   The current time
 *
 * @return true, if Max-Age of cache entry has passed.
 * @return false, if Max-Age of cache entry has not yet passed.
 */
static inline bool nanocoap_cache_entry_is_stale(const nanocoap_cache_entry_t *ce, uint32_t now)
{
    /* see https://en.wikipedia.org/w/index.php?title=Serial_number_arithmetic&oldid=1085516466#General_solution */
    return ((int)(now - ce->max_age) > 0);
}

#ifdef __cplusplus
}
#endif
/** @} */
