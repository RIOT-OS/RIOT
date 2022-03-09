/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * This file demonstrates how C code can be mixed with Rust code in an
 * application in an ad-hoc fashion.
 */

#include <vfs.h>
#include "fs/constfs.h"
#include <stdio.h>

#define HELLO_WORLD_CONTENT "Hello World!\n"
#define HELLO_RIOT_CONTENT  "Hello RIOT!\n"

#define LARGE  "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n" \
               "1234567890---------\n"

static constfs_file_t constfs_files[] = {
    {
        .path = "/hello-world",
        .size = sizeof(HELLO_WORLD_CONTENT),
        .data = (const uint8_t *)HELLO_WORLD_CONTENT,
    },
    {
        .path = "/hello-riot",
        .size = sizeof(HELLO_RIOT_CONTENT),
        .data = (const uint8_t *)HELLO_RIOT_CONTENT,
    },
    {
        .path = "/large",
        .size = sizeof(LARGE),
        .data = (const uint8_t *)LARGE,
    }
};

static constfs_t constfs_desc = {
    .nfiles = ARRAY_SIZE(constfs_files),
    .files = constfs_files,
};

static vfs_mount_t const_mount = {
    .fs = &constfs_file_system,
    .mount_point = "/const",
    .private_data = &constfs_desc,
};

void do_vfs_init(void) {
    int res = vfs_mount(&const_mount);
    if (res < 0) {
        puts("Error while mounting constfs");
    }
    else {
        puts("constfs mounted successfully");
    }
}
