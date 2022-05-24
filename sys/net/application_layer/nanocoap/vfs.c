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
 * @brief       Nanocoap VFS helpers
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <fcntl.h>
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _2file(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    (void)more;
    int *fd = arg;

    vfs_lseek(*fd, offset, SEEK_SET);
    return vfs_write(*fd, buf, len);
}

static int _prepare_file(const char *dst, char *dst_tmp, size_t len)
{
    /* download to temp file, rename it later */
    if (snprintf(dst_tmp, len, "%s.t", dst) > (int)len) {
        return -ENOBUFS;
    }

    return vfs_open(dst_tmp, O_CREAT | O_WRONLY | O_TRUNC, 0644);
}

static int _finalize_file(int fd, int res, const char *dst, const char *dst_tmp)
{
    vfs_close(fd);

    /* move file to it's final location */
    if (res >= 0) {
        DEBUG("nanocoap: moving %s to %s\n", dst_tmp, dst);
        vfs_unlink(dst);
        res = vfs_rename(dst_tmp, dst);
    }

    vfs_unlink(dst_tmp);

    return res;
}

int nanocoap_vfs_get(nanocoap_sock_t *sock, const char *path, const char *dst)
{
    int fd, res;
    char dst_tmp[CONFIG_SOCK_URLPATH_MAXLEN];

    DEBUG("nanocoap: downloading %s to %s\n", path, dst_tmp);

    fd = _prepare_file(dst, dst_tmp, sizeof(dst_tmp));
    if (fd < 0) {
        return fd;
    }
    res = nanocoap_sock_get_blockwise(sock, path, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                      _2file, &fd);
    return _finalize_file(fd, res, dst, dst_tmp);
}

int nanocoap_vfs_get_url(const char *url, const char *dst)
{
    int fd, res;
    char dst_tmp[CONFIG_SOCK_URLPATH_MAXLEN];

    DEBUG("nanocoap: downloading %s to %s\n", url, dst_tmp);

    fd = _prepare_file(dst, dst_tmp, sizeof(dst_tmp));
    if (fd < 0) {
        return fd;
    }
    res = nanocoap_get_blockwise_url(url, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                     _2file, &fd);
    return _finalize_file(fd, res, dst, dst_tmp);
}
