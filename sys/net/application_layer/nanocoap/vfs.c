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
 * @brief       nanoCoAP VFS helpers
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <fcntl.h>
#include "net/nanocoap_vfs.h"
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

    fd = _prepare_file(dst, dst_tmp, sizeof(dst_tmp));
    if (fd < 0) {
        return fd;
    }

    DEBUG("nanocoap: downloading %s to %s\n", path, dst_tmp);
    res = nanocoap_sock_get_blockwise(sock, path, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                      _2file, &fd);
    return _finalize_file(fd, res, dst, dst_tmp);
}

int nanocoap_vfs_get_url(const char *url, const char *dst)
{
    int fd, res;
    char dst_tmp[CONFIG_SOCK_URLPATH_MAXLEN];

    fd = _prepare_file(dst, dst_tmp, sizeof(dst_tmp));
    if (fd < 0) {
        return fd;
    }

    DEBUG("nanocoap: downloading %s to %s\n", url, dst_tmp);
    res = nanocoap_get_blockwise_url(url, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                     _2file, &fd);
    return _finalize_file(fd, res, dst, dst_tmp);
}

static int _vfs_put(coap_block_request_t *ctx, const char *file, void *buffer)
{
    int res, fd = vfs_open(file, O_RDONLY, 0644);
    if (fd < 0) {
        return fd;
    }

    /* buffer is at least one larger than SZX value */
    int buffer_len = coap_szx2size(ctx->blksize) + 1;

    bool more = true;
    while (more && (res = vfs_read(fd, buffer, buffer_len)) > 0) {
        more = res == buffer_len;
        res = nanocoap_sock_block_request(ctx, buffer,
                                          res, more, NULL, NULL);
        if (res < 0) {
            break;
        }
        vfs_lseek(fd, -1, SEEK_CUR);
    }

    vfs_close(fd);
    return res;
}

int nanocoap_vfs_put(nanocoap_sock_t *sock, const char *path, const char *src,
                     void *work_buf, size_t work_buf_len)
{
    DEBUG("nanocoap: uploading %s to %s\n", src, path);

    if (work_buf_len < coap_szx2size(0) + 1) {
        return -ENOBUFS;
    }

    coap_block_request_t ctx = {
        .path = path,
        .method = COAP_METHOD_PUT,
        .blksize = coap_size2szx(work_buf_len - 1),
        .sock = sock,
    };

    return _vfs_put(&ctx, src, work_buf);
}

int nanocoap_vfs_put_url(const char *url, const char *src,
                         void *work_buf, size_t work_buf_len)
{
    DEBUG("nanocoap: uploading %s to %s\n", src, url);

    if (work_buf_len < coap_szx2size(0) + 1) {
        return -ENOBUFS;
    }

    nanocoap_sock_t sock;
    coap_block_request_t ctx;
    int res = nanocoap_block_request_connect_url(&ctx, &sock, url, COAP_METHOD_PUT,
                                                 coap_size2szx(work_buf_len - 1));
    if (res == 0) {
        res = _vfs_put(&ctx, src, work_buf);
        nanocoap_sock_close(&sock);
    }

    return res;
}
