/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
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
 * @brief       nanoCoAP VFS backend
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <fcntl.h>

#include "net/nanocoap_sock.h"
#include "net/nanocoap/fs.h"
#include "net/nanocoap/link_format.h"
#include "net/sock/util.h"
#include "string_utils.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char _dirent_buf[CONFIG_NANOCOAP_QS_MAX];

static int nanocoap_fs_mount(vfs_mount_t *mountp)
{
    nanocoap_fs_t *fs = mountp->private_data;

    static_assert(VFS_FILE_BUFFER_SIZE >= sizeof(nanocoap_fs_file_t),
                  "nanocoap_fs_file_t must fit into VFS_FILE_BUFFER_SIZE");
    static_assert(VFS_DIR_BUFFER_SIZE >= sizeof(nanocoap_fs_dir_t),
                  "nanocoap_fs_dir_t must fit into VFS_DIR_BUFFER_SIZE");

    return nanocoap_sock_url_connect(fs->url, &fs->sock);
}

static int nanocoap_fs_umount(vfs_mount_t *mountp)
{
    nanocoap_fs_t *fs = mountp->private_data;

    nanocoap_sock_close(&fs->sock);
    return 0;
}

static int _fill_urlbuf(nanocoap_fs_t *fs, char dst[CONFIG_SOCK_URLPATH_MAXLEN],
                        const char *name, bool dir)
{
    name += 1; /* skip leading '/' */

    const char *extra = "";
    if (dir) {
        const char *end = name + strlen(name) - 1;
        if (*end != '/') {
            extra = "/";
        }
    }

    if ((unsigned)snprintf(dst, CONFIG_SOCK_URLPATH_MAXLEN, "%s%s%s",
                 sock_urlpath(fs->url), name, extra) > CONFIG_SOCK_URLPATH_MAXLEN) {
        DEBUG("nanocoap_fs: %s%s > %u bytes\n",
              sock_urlpath(fs->url), name, CONFIG_SOCK_URLPATH_MAXLEN);
        return -ENOBUFS;
    }

    return 0;
}

static int nanocoap_fs_unlink(vfs_mount_t *mountp, const char *name)
{
    nanocoap_fs_t *fs = mountp->private_data;
    int res;

    mutex_lock(&fs->lock);

    if ((res = _fill_urlbuf(fs, fs->urlbuf, name, true))) {
        goto out;
    }

    DEBUG("nanocoap_fs: delete %s\n", fs->urlbuf);
    res = nanocoap_sock_delete(&fs->sock, fs->urlbuf);

out:
    mutex_unlock(&fs->lock);
    return res;
}

static int nanocoap_fs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode)
{
    nanocoap_fs_t *fs = filp->mp->private_data;
    nanocoap_fs_file_t *file = (void *)filp->private_data.buffer;
    int res;

    (void)mode;

    if (flags != O_RDONLY) {
        /* so far only read is implemented */
        return -ENOSYS;
    }

    if ((res = _fill_urlbuf(fs, file->urlbuf, name, false))) {
        return res;
    }

    DEBUG("nanocoap_fs: open %s\n", file->urlbuf);

    file->offset = 0;

    return 0;
}

static ssize_t nanocoap_fs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    nanocoap_fs_t *fs = filp->mp->private_data;
    nanocoap_fs_file_t *file = (void *)filp->private_data.buffer;
    int res;

    DEBUG("nanocoap_fs: read %"PRIuSIZE" bytes\n", nbytes);

    res = nanocoap_sock_get_slice(&fs->sock, file->urlbuf, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                  file->offset, dest, nbytes);
    if (res > 0) {
        file->offset += res;
    }

    return res;
}

static off_t nanocoap_fs_lseek(vfs_file_t *filp, off_t off, int whence)
{
    nanocoap_fs_file_t *file = (void *)filp->private_data.buffer;

    switch (whence) {
    case SEEK_SET:
        file->offset = off;
        break;
    case SEEK_CUR:
        file->offset += off;
        break;
    case SEEK_END:
        return -ENOSYS;
    }

    return file->offset;
}

static int _block_cb(void *arg, coap_pkt_t *pkt)
{
    struct stat *restrict buf = arg;

    if (coap_get_code_class(pkt) != COAP_CLASS_SUCCESS) {
        return -ENXIO;
    }

    buf->st_mode = S_IFREG;

    uint32_t size = 0;
    coap_opt_get_uint(pkt, COAP_OPT_SIZE2, &size);
    buf->st_size = size;

    return 0;
}

static int _query_server(nanocoap_sock_t *sock, const char *path,
                         struct stat *restrict arg)
{
    uint8_t _buf[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    uint8_t *buf = _buf;

    coap_pkt_t pkt = {
        .buf = buf,
    };

    uint16_t lastonum = 0;

    ssize_t hdr_len = coap_build_udp_hdr(_buf, sizeof(_buf), COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET,
                                         nanocoap_sock_next_msg_id(sock));
    assume(hdr_len > 0);
    buf += hdr_len;
    buf += coap_opt_put_uri_pathquery(buf, &lastonum, path);
    buf += coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2, 0);
    buf += coap_opt_put_uint(buf, COAP_OPT_BLOCK2, COAP_OPT_SIZE2, 0);

    assert((uintptr_t)buf - (uintptr_t)_buf < sizeof(_buf));

    pkt.payload = buf;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _block_cb, arg);
}

static int nanocoap_fs_stat(vfs_mount_t *mountp, const char *restrict path,
                            struct stat *restrict buf)
{
    nanocoap_fs_t *fs = mountp->private_data;
    int res = 0;

    mutex_lock(&fs->lock);

    if ((res = _fill_urlbuf(fs, fs->urlbuf, path, false))) {
        goto out;
    }

    uint8_t dummy[4];
    if (_query_server(&fs->sock, fs->urlbuf, buf) < 0) {
        if ((res = _fill_urlbuf(fs, fs->urlbuf, path, true))) {
            goto out;
        }

        if (nanocoap_sock_get_slice(&fs->sock, fs->urlbuf, COAP_BLOCKSIZE_16,
                                    0, dummy, sizeof(dummy)) >= 0) {
            buf->st_mode = S_IFDIR;
        }
    }

out:
    mutex_unlock(&fs->lock);
    return res;
}

static int nanocoap_fs_opendir(vfs_DIR *dirp, const char *dirname)
{
    nanocoap_fs_t *fs = dirp->mp->private_data;
    nanocoap_fs_dir_t *dir = (void *)dirp->private_data.buffer;
    int res;

    if ((res = _fill_urlbuf(fs, dir->urlbuf, dirname, true))) {
        return res;
    }

    DEBUG("nanocoap_fs: opendir(%s)\n", dir->urlbuf);

    dir->offset = 0;

    return 0;
}

struct _dir_ctx {
    vfs_dirent_t *dirent;
    size_t offset;
};

static int _dir_cb(char *entry, void *arg)
{
    struct _dir_ctx *ctx = arg;

    if (ctx->offset) {
        --ctx->offset;
        return 0;
    }

    char *start = strchr(entry, '<');
    if (start) {
        char *end = strchr(entry, '>');
        *end = '\0';
        entry = start + 1;
    }

    char *end = entry + strlen(entry) - 1;
    if (*end == '/') {
        *end = '\0';
    }

    char *basename = strrchr(entry, '/');
    if (basename) {
        entry = basename + 1;
    }

    strscpy(ctx->dirent->d_name, entry, sizeof(ctx->dirent->d_name));
    return -EINTR;
}

static int nanocoap_fs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    nanocoap_fs_t *fs = dirp->mp->private_data;
    nanocoap_fs_dir_t *dir = (void *)dirp->private_data.buffer;
    int res;

    entry->d_ino = dir->offset;

    struct _dir_ctx ctx = {
        .dirent = entry,
        .offset = dir->offset++,
    };

    res = nanocoap_link_format_get(&fs->sock, dir->urlbuf, _dir_cb, &ctx,
                                   _dirent_buf, sizeof(_dirent_buf));
    if (res == -EINTR) {
        /* we use this to abort listing early */
        res = 1;
    }

    return res;
}

static const vfs_file_system_ops_t nanocoap_fs_ops = {
    .stat = nanocoap_fs_stat,
    .mount = nanocoap_fs_mount,
    .umount = nanocoap_fs_umount,
    .unlink = nanocoap_fs_unlink,
};

static const vfs_file_ops_t nanocoap_fs_file_ops = {
    .lseek = nanocoap_fs_lseek,
    .open  = nanocoap_fs_open,
    .read  = nanocoap_fs_read,
};

static const vfs_dir_ops_t nanocoap_fs_dir_ops = {
    .opendir = nanocoap_fs_opendir,
    .readdir = nanocoap_fs_readdir,
};

const vfs_file_system_t nanocoap_fs_file_system = {
    .f_op = &nanocoap_fs_file_ops,
    .fs_op = &nanocoap_fs_ops,
    .d_op = &nanocoap_fs_dir_ops,
};
