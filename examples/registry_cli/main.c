/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RIOT Registry example application
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "msg.h"
#include "shell.h"
#include "board.h"
#include "mtd.h"
#include "vfs.h"
#include "fs/littlefs2_fs.h"
#include "registry.h"
#include "registry/namespace/sys.h"
#include "registry/namespace/sys/rgb_led.h"
#include "registry/storage.h"
#include "registry/string_path.h"
#include "registry/namespace/tests.h"
#include "registry/namespace/tests/nested.h"

int rgb_led_instance_0_commit_cb(const registry_commit_cb_scope_t scope,
                                 const registry_group_or_parameter_id_t *group_or_parameter_id,
                                 const void *context)
{
    (void)scope;
    (void)context;
    printf("RGB instance commit_cb was executed on ");

    if (group_or_parameter_id != NULL) {
        printf("param: %d", *group_or_parameter_id);
    }
    else {
        printf("whole instance");
    }

    printf("\n");

    return 0;
}

registry_sys_rgb_led_instance_t rgb_led_instance_0_data = {
    .red = 0,
    .green = 255,
    .blue = 70,
};
registry_instance_t rgb_led_instance_0 = {
    .name = "rgb-0",
    .data = &rgb_led_instance_0_data,
    .commit_cb = &rgb_led_instance_0_commit_cb,
};

registry_sys_rgb_led_instance_t rgb_led_instance_1_data = {
    .red = 90,
    .green = 4,
    .blue = 0,
};
registry_instance_t rgb_led_instance_1 = {
    .name = "rgb-1",
    .data = &rgb_led_instance_1_data,
    .commit_cb = &rgb_led_instance_0_commit_cb,
};

static littlefs2_desc_t fs_desc = {
    .lock = MUTEX_INIT,
};

static vfs_mount_t _vfs_mount = {
    .fs = &littlefs2_file_system,
    .mount_point = "/sda",
    .private_data = &fs_desc,
};

static registry_storage_instance_t vfs_instance = {
    .storage = &registry_storage_vfs,
    .data = &_vfs_mount,
};

REGISTRY_ADD_STORAGE_SOURCE(vfs_instance);
REGISTRY_SET_STORAGE_DESTINATION(vfs_instance);

static registry_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_nested_instance = {
    .name = "instance-1",
    .data = &test_nested_instance_data,
    .commit_cb = NULL,
};

int main(void)
{
    registry_init();

    /* init schemas */
    registry_add_schema_instance(&registry_sys_rgb_led, &rgb_led_instance_0);
    registry_add_schema_instance(&registry_sys_rgb_led, &rgb_led_instance_1);
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance);

    /* init storage */
    #if IS_USED(MODULE_LITTLEFS2)
    fs_desc.dev = MTD_0;
    #endif

    /* init and run CLI */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, sizeof(line_buf));
    return 0;
}
