/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       NanoCoAP internals
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NANOCOAP_INTERNAL_H
#define NANOCOAP_INTERNAL_H

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal CoAP resource request handler context
 */
struct _coap_request_ctx {
    const coap_resource_t *resource;    /**< resource of the request */
    void *context;                      /**< request context, needed to supply
                                             the remote for the forward proxy */
#if defined(MODULE_GCOAP) || DOXYGEN
    /**
     * @brief   transport the packet was received over
     * @see     @ref gcoap_socket_type_t for values.
     * @note    @ref gcoap_socket_type_t can not be used, as this would
     *          cyclically include the @ref net_gcoap header.
     */
    uint32_t tl_type;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* NANOCOAP_INTERNAL_H */
/** @} */
