/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit
 * @defgroup    sys_suit_transport_coap SUIT firmware CoAP transport
 * @brief       SUIT secure firmware updates over CoAP
 *
 * @{
 *
 * @brief       SUIT CoAP helper API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef SUIT_TRANSPORT_COAP_H
#define SUIT_TRANSPORT_COAP_H

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Start SUIT CoAP thread
 */
void suit_coap_run(void);

/**
 * @brief SUIT CoAP endpoint entry.
 *
 * In order to use, include this header, then add SUIT_COAP_SUBTREE to the nanocoap endpoint array.
 * Mind the alphanumerical sorting!
 *
 * See examples/suit_update for an example.
 */
#define SUIT_COAP_SUBTREE \
    { \
        .path="/suit/", \
        .methods=COAP_MATCH_SUBTREE | COAP_METHOD_GET | COAP_METHOD_POST | COAP_METHOD_PUT, \
        .handler=coap_subtree_handler, \
        .context=(void*)&coap_resource_subtree_suit \
    }

/*
 * Dear Reviewer,
 *
 * At the time of PR'ing this code, there was a pile of CoAP PR's waiting for
 * reviews.  Some of that functionality is needed in one way or another for
 * SUIT. In order to not block software updates with CoAP refactoring, some of
 * the work-in-progress code has been copied here.  We expect this to be
 * removed as soon as CoAP in master provides similar functionality.
 *
 * As this is internal code that will go soon, I exclude this from Doxygen.
 *
 * Kaspar (July 2019)
 */
#ifndef DOXYGEN

/**
 * @brief   Coap subtree handler
 *
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       buf     Buffer to write reply to
 * @param[in]       len     Total length of the buffer associated with the
 *                          request
 * @param[in]       buf     Buffer to write reply to
 *
 * @returns     ssize_t     Size of the reply
 */
ssize_t coap_subtree_handler(coap_pkt_t *pkt, uint8_t *buf,
                             size_t len, void *context);

/**
 * @brief   Type for CoAP resource subtrees
 */
typedef const struct {
    const coap_resource_t *resources;   /**< ptr to resource array  */
    const size_t resources_numof;       /**< nr of entries in array */
} coap_resource_subtree_t;

/**
 * @brief   Coap blockwise request callback descriptor
 *
 * @param[in] arg      Pointer to be passed as arguments to the callback
 * @param[in] offset   Offset of received data
 * @param[in] buf      Pointer to the received data
 * @param[in] len      Length of the received data
 * @param[in] more     -1 for no option, 0 for last block, 1 for more blocks
 *
 * @returns    0       on success
 * @returns   -1       on error
 */
typedef int (*coap_blockwise_cb_t)(void *arg, size_t offset, uint8_t *buf, size_t len, int more);

/**
 * @brief   Reference to the coap resource subtree
 */
extern const coap_resource_subtree_t coap_resource_subtree_suit;

/**
 * @brief Coap block-wise-transfer size SZX
 */
typedef enum {
    COAP_BLOCKSIZE_32 = 1,
    COAP_BLOCKSIZE_64,
    COAP_BLOCKSIZE_128,
    COAP_BLOCKSIZE_256,
    COAP_BLOCKSIZE_512,
    COAP_BLOCKSIZE_1024,
} coap_blksize_t;

/**
 * @brief Coap block-wise-transfer size used for SUIT
 */
#ifndef CONFIG_SUIT_COAP_BLOCKSIZE
#define CONFIG_SUIT_COAP_BLOCKSIZE  COAP_BLOCKSIZE_64
#endif

/**
 * @brief    Performs a blockwise coap get request to the specified url.
 *
 * This function will fetch the content of the specified resource path via
 * block-wise-transfer. A coap_blockwise_cb_t will be called on each received
 * block.
 *
 * @param[in]   url        url pointer to source path
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   callback   callback to be executed on each received block
 * @param[in]   arg        optional function arguments
 *
 * @returns     -EINVAL    if an invalid url is provided
 * @returns     -1         if failed to fetch the url content
 * @returns      0         on success
 */
int suit_coap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg);

/**
 * @brief   Trigger a SUIT udate
 *
 * @param[in] url       url pointer containing the full coap url to the manifest
 * @param[in] len       length of the url
 */
void suit_coap_trigger(const uint8_t *url, size_t len);

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* SUIT_TRANSPORT_COAP_H */
/** @} */
