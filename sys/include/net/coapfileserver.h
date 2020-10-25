/*
 * Copyright (C) 2020 chrysn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_coapfileserver
 * @{
 *
 * @file
 * @brief       Resource handler for the CoAP file system server
 *
 * @author      chrysn <chrysn@fsfe.org>
 *
 * @}
 */

#ifndef NET_COAPFILESERVER_H
#define NET_COAPFILESERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net/nanocoap.h>

/** File server starting point
 *
 * This struct needs to be present at the ctx of a coapfileserver_handler entry
 * in a resource list.
 *
 */
struct coapfileserver_entry {
    /** Path in the VFS that should be served.
     *
     * This does not need a trailing slash. */
    char *nameprefix;
    /** Number of leading path components to ignore as they are the common prefix.
     *
     * If the file server is bound to the "/" resource, make this 0; if there
     * is an entry
     *
     * ``{ "/files/sd", COAP_GET | COAP_MATCH_SUBTREE, coapfileserver_handler, files_sd }``
     *
     * then ``files_sd.strip_path`` needs to be 2.
     *
     * */
    uint8_t strip_path;
};

/**
 * File server handler
 *
 * Serve a directory from the VFS as a CoAP resource tree.
 *
 * @p ctx pointer to a @ref coapfileserver_entry
 *
 * @see net_coapfileserver
 */
ssize_t coapfileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NET_COAPFILESERVER_H */

/** @} */
