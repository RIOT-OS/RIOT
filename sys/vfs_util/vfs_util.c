/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_vfs_util
 * @{
 * @file
 * @brief   VFS layer helper functions
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "vfs.h"
#include "vfs_util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int vfs_file_from_buffer(const char *file, const void *buf, size_t len)
{
    int res, fd = vfs_open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);

    if (fd < 0) {
        DEBUG("can't open %s for writing\n", file);
        return fd;
    }

    res = vfs_write(fd, buf, len);
    vfs_close(fd);

    if (res) {
        return res;
    }

    return 0;
}

int vfs_file_to_buffer(const char* file, void* buf, size_t len)
{
    int res, fd = vfs_open(file, O_RDONLY, 0);

    if (fd < 0) {
        DEBUG("can't open %s for reading\n", file);
        return fd;
    }

    res = vfs_read(fd, buf, len);

    /* ENOSPC is used to signal truncation */
    /* Just for future proofing - this error code is not returned by any fs in the read path. */
    if (res == -ENOSPC) {
        DEBUG("read returned -ENOSPC\n");
        res = -ENOMEM;
    }

    if (res > 0) {
        if (res < (int)len) {
            /* fill remaining buffer with 0 */
            memset((char *)buf + res, 0, len - res);
        } else {
            /* check if there are more bytes in the file */
            char c;
            if (vfs_read(fd, &c, sizeof(c)) > 0) {
                res = -ENOSPC;
            }
        }
    }

    vfs_close(fd);

    return res;
}

#if MODULE_HASHES
#include "hashes/md5.h"
#include "hashes/sha1.h"
#include "hashes/sha256.h"

int vfs_file_md5(const char* file, void *digest,
                 void *work_buf, size_t work_buf_len)
{
    md5_ctx_t ctx;

    int res, fd = vfs_open(file, O_RDONLY, 0);

    if (fd < 0) {
        DEBUG("can't open %s for reading\n", file);
        return fd;
    }

    md5_init(&ctx);
    while ((res = vfs_read(fd, work_buf, work_buf_len)) > 0) {
        md5_update(&ctx, work_buf, res);
    }
    md5_final(&ctx, digest);

    vfs_close(fd);

    return res > 0 ? 0 : res;
}

int vfs_file_sha1(const char* file, void *digest,
                  void *work_buf, size_t work_buf_len)
{
    sha1_context ctx;
    int res, fd = vfs_open(file, O_RDONLY, 0);

    if (fd < 0) {
        DEBUG("can't open %s for reading\n", file);
        return fd;
    }

    sha1_init(&ctx);
    while ((res = vfs_read(fd, work_buf, work_buf_len)) > 0) {
        sha1_update(&ctx, work_buf, res);
    }
    sha1_final(&ctx, digest);

    vfs_close(fd);

    return res > 0 ? 0 : res;

}

int vfs_file_sha256(const char* file, void *digest,
                    void *work_buf, size_t work_buf_len)
{
    sha256_context_t ctx;
    int res, fd = vfs_open(file, O_RDONLY, 0);

    if (fd < 0) {
        DEBUG("can't open %s for reading\n", file);
        return fd;
    }

    sha256_init(&ctx);
    while ((res = vfs_read(fd, work_buf, work_buf_len)) > 0) {
        sha256_update(&ctx, work_buf, res);
    }
    sha256_final(&ctx, digest);

    vfs_close(fd);

    return res > 0 ? 0 : res;

}
#endif /* MODULE_HASHES */
