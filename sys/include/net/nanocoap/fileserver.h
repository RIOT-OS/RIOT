/*
 * SPDX-FileCopyrightText: 2020 chrysn
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_nanocoap_fileserver CoAP file server
 * @ingroup     net_nanocoap
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
 * * ``USEMODULE += nanocoap_fileserver``
 *
 * * Enter a @ref nanocoap_fileserver_handler handler into your CoAP server's
 *   resource list like this:
 *
 *   ```
 *   static const coap_resource_t _resources[] = {
 *       ...
 *       {
 *          .path = "/files/sd",
 *          .methods = COAP_GET | COAP_MATCH_SUBTREE,
 *          .handler = nanocoap_fileserver_handler,
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
 *   ``nanocoap_fileserver_put`` and ``nanocoap_fileserver_delete``.
 *
 * @{
 *
 * @file
 * @brief       Resource handler for the CoAP file system server
 *
 * @author      chrysn <chrysn@fsfe.org>
 */

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
 * @note This requires the nanocoap_fileserver_callback module.
 */
typedef enum {
    NANOCOAP_FILESERVER_GET_FILE_START, /**< file download started   */
    NANOCOAP_FILESERVER_GET_FILE_END,   /**< file download finished  */
    NANOCOAP_FILESERVER_PUT_FILE_START, /**< file upload started     */
    NANOCOAP_FILESERVER_PUT_FILE_END,   /**< file upload finished    */
    NANOCOAP_FILESERVER_DELETE_FILE,    /**< file deletion requested
                                         (called before file is deleted) */
} nanocoap_fileserver_event_t;

/**
 * @brief   GCoAP fileserver event context
 */
typedef struct {
    const char *path;               /**< VFS path of the affected file  */
    void *user_ctx;                 /**< Optional user supplied context */
} nanocoap_fileserver_event_ctx_t;

/**
 * @brief   GCoAP fileserver event callback type
 *
 * @param[in] event     Type of the event
 * @param[in] ctx       Event context information
 *
 */
typedef void (*nanocoap_fileserver_event_handler_t)(nanocoap_fileserver_event_t event,
                                                    nanocoap_fileserver_event_ctx_t *ctx);

/**
 * @brief   Register a consumer for GCoAP fileserver events
 *          Requires the `nanocoap_fileserver_callback` module
 *
 *          The Callback is called on each fileserver event and executed
 *          within the GCoAP thread.
 *
 * @param[in]   cb  The callback function to be called on events
 * @param[in]  arg  Custom callback function context
 *
 */
void nanocoap_fileserver_set_event_cb(nanocoap_fileserver_event_handler_t cb, void *arg);

/**
 * @brief File server handler
 *
 * Serve a directory from the VFS as a CoAP resource tree.
 * @see net_nanocoap_fileserver
 *
 * @param[in] pdu   CoAP request package
 * @param[out] buf  Buffer for the response
 * @param[in] len   Response buffer length
 * @param[in] ctx   pointer to a @ref coap_request_ctx_t
 *
 * @return size of the response on success
 *         negative error
 */
ssize_t nanocoap_fileserver_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                    coap_request_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

/** @} */
