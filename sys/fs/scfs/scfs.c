/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <errno.h>

#include "fs.h"
#include "scfs.h"
#include "shell_commands.h"

static int busy = 0;

int scfs_mount(const char *mount_point)
{
    if (busy) {
        errno = EBUSY;
        return -EBUSY;
    }

    return fs_mount(mount_point, 0, FS_SCFS);
}

int scfs_unmount(const char *mount_point)
{
    if (!busy) {
        return 0;
    }

    return fs_unmount(mount_point, 0);
}
