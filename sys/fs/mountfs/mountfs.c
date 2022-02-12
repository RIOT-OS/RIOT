/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fs_mountfs MountFS static file system
 * @ingroup     sys_fs
 * @{
 *
 * @file
 * @brief       mountfs implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "clist.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

clist_node_t *vfs_get_mounts(void);

static vfs_mount_t const_mount;

typedef struct {
    clist_node_t *node;
    clist_node_t *start;
} mountfs_ctx_t;

static int _opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    (void)abs_path;
    if (strncmp(dirname, "/", 2) != 0) {
        return -ENOENT;
    }

    mountfs_ctx_t *ctx = (void *)dirp->private_data.buffer;
    ctx->node = vfs_get_mounts();
    ctx->start = ctx->node;
    return 0;
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    while (1) {
        mountfs_ctx_t *ctx = (void *)dirp->private_data.buffer;
        if (ctx->node == NULL) {
            return 0;
        }
        vfs_mount_t *it = container_of(ctx->node, vfs_mount_t, list_entry);

        ctx->node = ctx->node->next;
        if (ctx->node == ctx->start) {
            ctx->node = NULL;
        }

        if (it->mount_point == NULL) {
            continue;
        }

        if (it == &const_mount) {
            continue;
        }

        strncpy(entry->d_name, it->mount_point + 1, VFS_NAME_MAX);
        break;
    };

    return 1;
}

static int _closedir(vfs_DIR *dirp)
{
    mountfs_ctx_t *ctx = (void *)dirp->private_data.buffer;
    ctx->node = NULL;

    return 0;
}

static const vfs_file_system_ops_t mountfs_fs_ops;
static const vfs_file_ops_t mountfs_file_ops;
static const vfs_dir_ops_t mountfs_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

static const vfs_file_system_t mountfs_file_system = {
    .f_op = &mountfs_file_ops,
    .fs_op = &mountfs_fs_ops,
    .d_op = &mountfs_dir_ops,
};

static vfs_mount_t const_mount = {
    .fs = &mountfs_file_system,
    .mount_point = "/",
};

void auto_init_mountfs(void)
{
    vfs_mount(&const_mount);
}
