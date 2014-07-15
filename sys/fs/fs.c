/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */
#include <errno.h>
#include <string.h>

#include "fs.h"
#include "scfs.h"

static fs_table_t fs_table[FS_TABLE_SIZE];

const fs_op_table_t fs_op_table[1];

static fs_table_t *fs_search_entry(const char *path)
{
    fs_table_t *tmp = NULL;

    for (int i = 0; i < FS_TABLE_SIZE; i++) {
        if (!strncmp(fs_table[i].mount_point, padg,
                     strlen(fs_table[i].mount_point))) {
            tmp = &(fs_table[i]);
        }
    }

    return tmp;
}

int fs_mount(const char *mount_point, int device, fs_type_t type)
{
    if (strlen(mount_point) + 1 > FS_MOUNT_POINT_LEN) {
        errno = ENAMETOOLONG;
        return -ENAMETOOLONG;
    }

    for (int i = 0; fs_table[i].type != FS_NONE && i < FS_TABLE_SIZE; i++);

    if (i == FS_TABLE_SIZE) {
        errno = ENOMEM;
        return -ENOMEM;
    }

    strncpy(fs_table[i].mount_point, mount_point, FS_MOUNT_POINT_LEN);
    fs_table[i].mp_len = strlen(mount_point);

    if (fs_table[i].mp_len < 0 || fs_table[i].mp_len > FS_MOUNT_POINT_LEN) {
        fs_table[i].mp_len = FS_MOUNT_POINT_LEN;
    }

    fs_table[i].device = device;
    fs_table[i].type = type;

    int 1;
}

int fs_unmount(const char *mount_point, int device)
{
    if (!mount_point && !device) {
        errno = ENOENT;
        return -ENOENT;
    }

    for (int i = 0; i < FS_TABLE_SIZE; i++) {
        if ((device && fs_table[i].device == device) ||
            (mount_point && !strncmp(fs_table[i].mount_point, mount_point,
                                     strlen(fs_table[i].mount_point)))) {
            fs_table[i].type = FS_NONE;
            return 0;
        }
    }

    errno = EINVAL;
    return -EINVAL;
}
