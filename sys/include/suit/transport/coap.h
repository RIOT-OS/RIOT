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
#include "suit/transport/worker.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief   Reference to the coap resource subtree
 */
extern const coap_resource_subtree_t coap_resource_subtree_suit;

/**
 * @brief Coap block-wise-transfer size used for SUIT
 */
#ifndef CONFIG_SUIT_COAP_BLOCKSIZE
#define CONFIG_SUIT_COAP_BLOCKSIZE  CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT
#endif

/**
 * @brief   Trigger a SUIT udate
 *
 * @deprecated This is an alias for @ref suit_worker_trigger and will be removed
 *             after after the 2023.01 release.
 *
 * @param[in] url       url pointer containing the full coap url to the manifest
 * @param[in] len       length of the url
 */
static inline void suit_coap_trigger(const uint8_t *url, size_t len)
{
    suit_worker_trigger((const char *)url, len);
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SUIT_TRANSPORT_COAP_H */
