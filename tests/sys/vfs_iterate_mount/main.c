/*
 * Copyright (C) Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * Mount and unmount a few file systems, demonstrating that
 * vfs_iterate_mount_dirs performs as advertised.
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#include <stdio.h>
#include <string.h>

#include <vfs.h>
#include <fs/constfs.h>

#include "test_utils/expect.h"

static constfs_file_t constfs_files[1] = {
    /* Not completely empty -- that'd be a hassle around empty arrays and
     * their size */
    {
        .path = "some-file",
        .size = 0,
        .data = (void*)"",
    },
};

static constfs_t constfs_desc = {
    .nfiles = ARRAY_SIZE(constfs_files),
    .files = constfs_files,
};

static vfs_mount_t mount1 = {
    .fs = &constfs_file_system,
    .mount_point = "/const1",
    .private_data = &constfs_desc,
};

static vfs_mount_t mount2 = {
    .fs = &constfs_file_system,
    .mount_point = "/const2",
    .private_data = &constfs_desc,
};

static vfs_mount_t mount3 = {
    .fs = &constfs_file_system,
    .mount_point = "/const3",
    .private_data = &constfs_desc,
};

static vfs_mount_t mount4 = {
    .fs = &constfs_file_system,
    .mount_point = "/const4",
    .private_data = &constfs_desc,
};

/* Crank the iterator, reporting "N%s" for the next entry, or "O\n" for the end
 * of the iterator (avoiding the letter "E" which may be misread for an error
 * in a casual look at the error output) */
static void iter_and_report(vfs_DIR *iter) {
    bool result = vfs_iterate_mount_dirs(iter);
    if (result) {
        printf("N(%s)", iter->mp->mount_point);
    } else {
        printf("O\n");
        /* Zero out so we're ready for next round immediately */
        memset(iter, 0, sizeof(*iter));
    }
}

int main(void) {
    int res = 0;

    vfs_DIR iter;
    memset(&iter, 0, sizeof(iter));

    res |= vfs_mount(&mount1);
    res |= vfs_mount(&mount2);
    res |= vfs_mount(&mount3);
    res |= vfs_mount(&mount4);
    expect(res == 0);
    printf("Mounted 1234\n");

    /* N1N2N3N4E */
    iter_and_report(&iter);
    iter_and_report(&iter);
    iter_and_report(&iter);
    iter_and_report(&iter);
    iter_and_report(&iter);

    /* N1N2, unmount 3, N4E */
    iter_and_report(&iter);
    iter_and_report(&iter);
    res |= vfs_umount(&mount3, false);
    iter_and_report(&iter);
    iter_and_report(&iter);

    /* N1, unmount 2, (3 is already unmounted), N4, mount 3 N3, unmount 1 and remount it at the end N1, O */
    /* It is OK that 1 is reported twice, because its first occurrence is its
     * old mounting, and later it reappears */
    iter_and_report(&iter);
    res |= vfs_umount(&mount2, false);
    iter_and_report(&iter);
    res |= vfs_mount(&mount3);
    iter_and_report(&iter);
    res |= vfs_umount(&mount1, false);
    res |= vfs_mount(&mount1);
    iter_and_report(&iter);
    iter_and_report(&iter);

    /* This ensures we're not leaking locks */
    res |= vfs_umount(&mount1, false);
    res |= vfs_umount(&mount3, false);
    res |= vfs_umount(&mount4, false);
    printf("All unmounted\n");

    /* Only O */
    iter_and_report(&iter);
}
