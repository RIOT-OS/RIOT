/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_transport_vfs
 * @{
 *
 * @fil
 * @brief       SUIT VFS
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <fcntl.h>
#include <string.h>

#include "suit/transport/vfs.h"
#include "log.h"
#include "vfs.h"

int suit_transport_vfs_fetch(const suit_manifest_t *manifest, coap_blockwise_cb_t cb, void *ctx)
{
    const char *file = &manifest->urlbuf[7];
    size_t offset = 0;
    int res, fd;

    LOG_DEBUG("suit_vfs: read firmware from %s\n", file);

    fd = vfs_open(file, O_RDONLY, 0);
    if (fd < 0) {
        return fd;
    }

    void *buf = manifest->urlbuf;
    size_t max_len = manifest->urlbuf_len;
    while ((res = vfs_read(fd, buf, max_len)) > 0) {
        size_t len = res;
        res = cb(ctx, offset, buf, len, 1);
        if (res < 0) {
            LOG_ERROR("suit_vfs: write failed with %d\n", res);
            break;
        }
        offset += len;
    }

    if (res < 0) {
        LOG_ERROR("suit_vfs: read failed with %d\n", res);
    } else {
        res = cb(ctx, offset, buf, 0, 0);
    }

    vfs_close(fd);

    return res;
}
