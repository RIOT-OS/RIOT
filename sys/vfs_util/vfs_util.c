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

int vfs_is_dir(const char *path)
{
    assert(path);

    int err;
    struct stat stat;
    if (*path != '/') {
        /* only accept absolute paths */
        return -EINVAL;
    }
    if ((err = vfs_stat(path, &stat)) < 0) {
        return err;
    }
    return ((stat.st_mode & S_IFMT) == S_IFDIR);
}

bool vfs_file_exists(const char *path)
{
    int res = vfs_open(path, O_RDONLY, 0);
    if (res < 0) {
        return false;
    }

    vfs_close(res);
    return true;
}

/**
 * @brief   Removes additional "/" slashes from @p path
 *
 * @param[in]   path    Path to be prepared
 */
static void _vfs_prepare_path(char *path)
{
    assert(path);
    assert(*path == '/');

    int path_len = strlen(path);
    char *p_write = path; /* end of so far constructed path */
    int len = 0;
    const char *p_read = p_write; /* segment to be appended to the path */
    while (p_read < path + path_len) {
        len = 0;
        while (*p_read && *p_read == '/') {
            p_read++; /* skip slashes */
        }
        while (p_read[len] && p_read[len] != '/') {
            len++; /* length of segment to be copied */
        }
        if (*p_read && p_write + len + 1 <= path + path_len) {
            memmove(p_write + 1, p_read, len);
            p_write = p_write + len + 1; /* advance write pointer by segment length + 1 */
            *p_write = p_read[len]; /* either '\0' or '/' */
        }
        p_read += len; /* advance read pointer by segment length */
    }
    if (*p_write) {
        *++p_write = '\0';
    }
}

int vfs_unlink_recursive(const char *root, char *path_buf, size_t max_size)
{
    assert(root);
    assert(path_buf);

    /* This function works like a Depth-first search (DFS).
       First, we go as deep as we can into a directory and delete contained files.
       Then we delete the now empty directory and go to the parent directory
       and repeat the process. */
    int err;
    if (*root != '/' || !strcmp(root, "/")) {
        /* only accept absolute paths and not the FS root */
        return -EINVAL;
    }
    if (strlen(root) >= max_size) {
        return -ENOBUFS;
    }
    strcpy(path_buf, root);
    _vfs_prepare_path(path_buf);
    if (path_buf[strlen(path_buf) - 1] != '/') {
        if ((err = vfs_is_dir(path_buf)) < 0) {
            return err; /* early unexpected error */
        }
        else if (!err) {
            /* just a file */
            return vfs_unlink(path_buf);
        }
        strcat(path_buf, "/");
    }
    vfs_DIR dir;
    vfs_dirent_t entry;
    char seg[VFS_NAME_MAX + 1] = {0}; /* + 1 to append a '/' */
    size_t seg_len, root_len, fin = strlen(path_buf);
    while ((root_len = strlen(path_buf)) >= fin) {
        strcat(path_buf, seg);
        *seg = '\0';
        if ((err = vfs_opendir(&dir, path_buf)) < 0) { /* this works with a trailing '/' */
            return err;
        }
        while (vfs_readdir(&dir, &entry) > 0) {
            if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, "..")) {
                continue;
            }
            seg_len = strlen(entry.d_name);
            root_len = strlen(path_buf);
            if (root_len + seg_len >= max_size) {
                vfs_closedir(&dir);
                return -ENOBUFS;
            }
            strcat(path_buf, entry.d_name);
            if ((err = vfs_is_dir(path_buf)) < 0) {
                /* error */
                vfs_closedir(&dir);
                return err;
            }
            else if (err) {
                /* is dir */
                if (*seg == '\0') {
                    strcat(seg, entry.d_name);
                    strcat(seg, "/");
                }
            }
            else {
                /* is file */
                if ((err = vfs_unlink(path_buf)) < 0) {
                    vfs_closedir(&dir);
                    return err;
                }
            }
            path_buf[root_len] = '\0';
        }
        vfs_closedir(&dir);
        if (*seg == '\0') {
            /* no files and no subdirectory */
            if ((err = vfs_rmdir(path_buf)) < 0) {
                return err;
            }
            /* go one segment up */
            char *end = &path_buf[strlen(path_buf) - 1];
            assert(*end == '/');
            while (*--end != '/') { }
            *++end = '\0';
        }
    }
    return 0;
}
