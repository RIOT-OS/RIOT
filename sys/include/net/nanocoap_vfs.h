/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     net_nanosock
 * @brief       VFS NanoCoAP helper functions
 *
 * @{
 *
 * @file
 * @brief       VFS NanoCoAP helper functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/nanocoap_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Downloads the resource behind @p url via blockwise
 *          GET and stores it in the file @p dst.
 *
 * @param[in]   url     URL to the resource
 * @param[in]   dst     Path to the destination file
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_vfs_get_url(const char *url, const char *dst);

/**
 * @brief   Downloads the resource behind @p path via blockwise
 *          GET and stores it in the file @p dst.
 *
 * @param[in]   sock    Connection to the server
 * @param[in]   path    Remote query path to the resource
 * @param[in]   dst     Local path to the destination file
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_vfs_get(nanocoap_sock_t *sock, const char *path, const char *dst);

/**
 * @brief   Uploads the @p file to @p url via blockwise PUT.
 *
 * @param[in]   url          URL to the resource
 * @param[in]   src          Path to the source file
 * @param[in]   work_buf     Buffer to read file blocks into
 * @param[in]   work_buf_len Size of the buffer. Should be 1 byte more
 *                           than the desired CoAP blocksize.
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_vfs_put_url(const char *url, const char *src,
                         void *work_buf, size_t work_buf_len);

/**
 * @brief   Uploads the @p file to @p path via blockwise PUT.
 *
 * @param[in]   sock         Connection to the server
 * @param[in]   path         Remote query path to the resource
 * @param[in]   src          Path to the source file
 * @param[in]   work_buf     Buffer to read file blocks into
 * @param[in]   work_buf_len Size of the buffer. Should be 1 byte more
 *                           than the desired CoAP blocksize.
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_vfs_put(nanocoap_sock_t *sock, const char *path, const char *src,
                     void *work_buf, size_t work_buf_len);

#ifdef __cplusplus
}
#endif
/** @} */
