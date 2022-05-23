/*
 * Copyright (C) 2020 chrysn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gcoap_fileserver GCoAP file server
 * @ingroup     net_gcoap
 * @brief       Library for serving files from the VFS to CoAP clients
 *
 * # About
 *
 * This maps files in the local file system onto a resources in CoAP. In that,
 * it is what is called a static web server in the unconstrained web.
 *
 * As usual, GET operations are used to read files<!-- WRITESUPPORT, and PUT writes to files.
 * In the current implementation, PUTs are expressed as random-access, meaning
 * that files are not updated atomically -->.
 *
 * Directories are expressed to URIs with trailing slashes<!-- WRITESUPPORT, and are always
 * created implicitly when files are PUT under them; they can be DELETEd when
 * empty -->.
 *
 * @note The file server uses ETag for cache validation. The ETags are built
 * from the file system stat values. As clients rely on the ETag to differ when
 * the file changes, it is important that file modification times are set. The
 * precise time values do not matter, but if a file is changed in place and
 * neither its length nor its modification time is varied, then clients will
 * not become aware of the change or may even mix up the versions half way
 * through if they have a part of the old version cached.
 *
 * # Usage
 *
 * * ``USEMODULE += gcoap_fileserver``
 *
 * * Have a @ref gcoap_fileserver_entry_t populated with the path you want to serve,
 *   and the number of path components to strip from incoming requests:
 *
 *   ```
 *   static const gcoap_fileserver_entry_t files_sd = {
 *       .root = "/sd0",
 *       .resource = "/files/sd"
 *   };
 *   ```
 *
 * * Enter a @ref gcoap_fileserver_handler handler into your CoAP server's
 *   resource list like this:
 *
 *   ```
 *   static const coap_resource_t _resources[] = {
 *       ...
 *       { "/files/sd", COAP_GET | COAP_MATCH_SUBTREE, gcoap_fileserver_handler, (void*)&files_sd },
 *       ...
 *   }
 *   ```
 *
 *   The allowed methods dictate whether it's read-only (``COAP_GET``) or (in the
 *   future<!-- WRITESUPPORT -->) read-write (``COAP_GET | COAP_PUT | COAP_DELETE``).
 *
 * @{
 *
 * @file
 * @brief       Resource handler for the CoAP file system server
 *
 * @author      chrysn <chrysn@fsfe.org>
 */

#ifndef NET_GCOAP_FILESERVER_H
#define NET_GCOAP_FILESERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/nanocoap.h"

/**
 * @brief File server starting point
 *
 * This struct needs to be present at the ctx of a gcoap_fileserver_handler entry
 * in a resource list.
 *
 */
typedef struct {
    /**
     * @brief Path in the VFS that should be served.
     *
     * This does not need a trailing slash.
     */
    const char *root;
    /**
     * @brief The associated CoAP resource path
     */
    const char *resource;
} gcoap_fileserver_entry_t;

/**
 * @brief File server handler
 *
 * Serve a directory from the VFS as a CoAP resource tree.
 * @see net_gcoap_fileserver
 *
 * @param[in] pdu   CoAP request package
 * @param[out] buf  Buffer for the response
 * @param[in] len   Response buffer length
 * @param[in] ctx   pointer to a @ref gcoap_fileserver_entry_t
 *
 * @return size of the response on success
 *         negative error
 */
ssize_t gcoap_fileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_FILESERVER_H */

/** @} */
