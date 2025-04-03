/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_NANOCOAP_FS_H
#define NET_NANOCOAP_FS_H
/**
 * @ingroup     net_nanosock
 * @brief       nanoCoAP virtual file system
 *
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "mutex.h"
#include "net/nanocoap_sock.h"
#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief nanoCoAP file system configuration
 */
typedef struct {
    const char *url;                         /**< base URL of the remote fs */
    nanocoap_sock_t sock;                    /**< connection to the remote server */
    mutex_t lock;                            /**< lock for common urlbuf */
    char urlbuf[CONFIG_SOCK_URLPATH_MAXLEN]; /**< shared url buffer */
} nanocoap_fs_t;

/**
 * @brief nanoCoAP remote file struct
 */
typedef struct {
    uint32_t offset;                         /**< offset into the file  */
    char urlbuf[CONFIG_SOCK_URLPATH_MAXLEN]; /**< full path to the file */
} nanocoap_fs_file_t;

/**
 * @brief nanoCoAP remote dir struct
 */
typedef struct {
    uint32_t offset;                         /**< current directory element  */
    char urlbuf[CONFIG_SOCK_URLPATH_MAXLEN]; /**< full path of the directory */
} nanocoap_fs_dir_t;

/**
 * @brief nanoCoAP file system driver
 *
 * For use with vfs_mount
 */
extern const vfs_file_system_t nanocoap_fs_file_system;

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NET_NANOCOAP_FS_H */
