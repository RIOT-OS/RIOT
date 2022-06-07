/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_transport_vfs SUIT secure firmware OTA VFS transport
 * @ingroup     sys_suit
 * @brief       SUIT firmware VFS transport
 *
 *              Allows to load firmware updates from the filesystem.
 *              URL scheme: file://<path>/<to>/manifest.suit
 *
 *              e.g. set `SUIT_COAP_ROOT` to `file:///sd0/fw` and place the
 *              update files to the folder fw/ on the first SD card.
 * @{
 *
 * @brief       VFS transport backend definitions for SUIT manifests
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */

#ifndef SUIT_TRANSPORT_VFS_H
#define SUIT_TRANSPORT_VFS_H

#include "net/nanocoap.h"
#include "suit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief fetch a payload from the filesystem
 *
 * @param[in]   manifest    suit manifest context
 * @param[in]   cb          filesystem block callback
 * @param[in]   ctx         callback context
 *
 * @returns     SUIT_OK if valid
 * @returns     negative otherwise
 */
int suit_transport_vfs_fetch(const suit_manifest_t *manifest, coap_blockwise_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_TRANSPORT_VFS_H */
/** @} */
