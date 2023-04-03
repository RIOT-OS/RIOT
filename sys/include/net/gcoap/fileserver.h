/*
 * Copyright (C) 2020 chrysn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gcoap_fileserver gcoap_fileserver: GCoAP file server
 * @ingroup     net_gcoap
 * @brief       Library for serving files from the VFS to CoAP clients
 *
 * # About
 *
 * This maps files in the local file system onto a resources in CoAP. In that,
 * it is what is called a static web server in the unconstrained web.
 *
 * As usual, GET operations are used to read files and PUT writes to files.
 * In the current implementation, PUTs are expressed as random-access, meaning
 * that files are not updated atomically, although files are created atomically.
 * The Content-Format option is not checked in the current implementation.
 * Conditional file modification and deletion is supported using the If-Match
 * option. The If-Match option carries a previously received Etag or in case of
 * zero length, requires a request to be processed only if the resource exists.
 * In opposite, the If-None-Match option requires a request to be processed,
 * only if the resource does not yet exist, and is most useful for file creation.
 *
 * Directories are expressed to URIs with trailing slashes. Directories and
 * their content are deleted as if one would do an `$rm -r`. If you only would
 * like to delete a directory if it is empty, you must supply an If-Match option
 * with the special value @ref COAPFILESERVER_DIR_DELETE_ETAG.
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
 * * Enter a @ref gcoap_fileserver_handler handler into your CoAP server's
 *   resource list like this:
 *
 *   ```
 *   static const coap_resource_t _resources[] = {
 *       ...
 *       {
 *          .path = "/files/sd",
 *          .methods = COAP_GET | COAP_MATCH_SUBTREE,
 *          .handler = gcoap_fileserver_handler,
 *          .context = "/sd0"
 *       },
 *       ...
 *   }
 *   ```
 *
 *   The path argument specifies under which path the folder is served via CoAP while
 *   the context argument contains the path on the local filesystem that will be served.
 *
 *   The allowed methods dictate whether it's read-only (``COAP_GET``) or
 *   read-write (``COAP_GET | COAP_PUT | COAP_DELETE``).
 *   If you want to support ``PUT`` and `DELETE`, you need to enable the modules
 *   ``gcoap_fileserver_put`` and ``gcoap_fileserver_delete``.
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
 * @brief   Randomly generated Etag, used by a client when a directory should only be
 *          deleted, if it is empty
 */
#define COAPFILESERVER_DIR_DELETE_ETAG (0x6ce88b56u)

/**
 * @brief   GCoAP fileserver event types
 *
 * @note This requires the gcoap_fileserver_callback module.
 */
typedef enum {
    GCOAP_FILESERVER_GET_FILE_START,     /**< file download started   */
    GCOAP_FILESERVER_GET_FILE_END,       /**< file download finished  */
    GCOAP_FILESERVER_PUT_FILE_START,     /**< file upload started     */
    GCOAP_FILESERVER_PUT_FILE_END,       /**< file upload finished    */
    GCOAP_FILESERVER_DELETE_FILE,        /**< file deletion requested
                                         (called before file is deleted) */
} gcoap_fileserver_event_t;

/**
 * @brief   GCoAP fileserver event context
 */
typedef struct {
    const char *path;               /**< VFS path of the affected file  */
    void *user_ctx;                 /**< Optional user supplied context */
} gcoap_fileserver_event_ctx_t;

/**
 * @brief   GCoAP fileserver event callback type
 *
 * @param[in] event     Type of the event
 * @param[in] ctx       Event context information
 *
 */
typedef void (*gcoap_fileserver_event_handler_t)(gcoap_fileserver_event_t event,
                                                 gcoap_fileserver_event_ctx_t *ctx);

/**
 * @brief   Register a consumer for GCoAP fileserver events
 *          Requires the `gcoap_fileserver_callback` module
 *
 *          The Callback is called on each fileserver event and executed
 *          within the GCoAP thread.
 *
 * @param[in]   cb  The callback function to be called on events
 * @param[in]  arg  Custom callback function context
 *
 */
void gcoap_fileserver_set_event_cb(gcoap_fileserver_event_handler_t cb, void *arg);

/**
 * @brief File server handler
 *
 * Serve a directory from the VFS as a CoAP resource tree.
 * @see net_gcoap_fileserver
 *
 * @param[in] pdu   CoAP request package
 * @param[out] buf  Buffer for the response
 * @param[in] len   Response buffer length
 * @param[in] ctx   pointer to a @ref coap_request_ctx_t
 *
 * @return size of the response on success
 *         negative error
 */
ssize_t gcoap_fileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_FILESERVER_H */

/** @} */
