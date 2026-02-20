/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_int_path_storage_vfs RIOT Registry Path Storage: VFS
 * @ingroup     sys
 * @brief       RIOT Registry Path Storage VFS, allows using the RIOT VFS module as a RIOT Registry data storage.
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "vfs.h"
#include "ps.h"
#include "registry/int_path.h"

#include "registry/storage.h"

static registry_error_t load(const registry_storage_instance_t *storage,
                             const load_cb_t load_cb);
static registry_error_t save(const registry_storage_instance_t *storage,
                             const registry_node_t *node,
                             const registry_value_t *value);

registry_storage_t registry_storage_vfs = {
    .load = load,
    .save = save,
};

static void _string_path_append_item(
    char *dest,
    uint32_t number)
{
    int size = snprintf(NULL, 0, "/%" PRIu32, number);

    /* Allocate size + string termination */
    char buf[size + 1];

    sprintf(buf, "/%" PRIu32, number);

    strcat(dest, buf);
}

static int _format(vfs_mount_t *mount)
{
    DEBUG("formatting %s....\t", mount->mount_point);
    if (vfs_format(mount) < 0) {
        DEBUG("[Failed]\n");
        return 1;
    }
    else {
        DEBUG("[OK]\n");
    }

    return 0;
}

static int _mount(vfs_mount_t *mount)
{
    int res = vfs_mount(mount);

    if (res < 0) {
        DEBUG("Error while mounting %s ... (%d) ... try format\n", mount->mount_point, res);

        /* format to fix mount */
        _format(mount);

        /* try to mount again */
        res = vfs_mount(mount);
        if (res != 0) {
            return -1;
        }
    }

    return 0;
}

static int _umount(vfs_mount_t *mount)
{
    int res = vfs_umount(mount, false);

    if (res < 0) {
        DEBUG("Error while unmounting %s...\n", mount->mount_point);
        return 1;
    }

    return 0;
}

static registry_error_t load(
    const registry_storage_instance_t *storage,
    const load_cb_t load_cb)
{
    vfs_mount_t *mount = storage->data;

    /* mount */
    _mount(mount);

    /* create dir path */
    char string_path[REGISTRY_INT_PATH_STRING_MAX_LEN];

    sprintf(string_path, "%s", mount->mount_point);

    /* read dirs */
    vfs_DIR dirp;

    if (vfs_opendir(&dirp, string_path) != 0) {
        DEBUG("[registry storage_vfs] load: Can not open dir\n");
    }
    else {
        struct stat _stat;
        vfs_dirent_t dir_entry;

        size_t i = 0;
        int last_dir_entry_positions[REGISTRY_INT_PATH_MAX_LEN] = { -1 };
        size_t last_dir_string_path_lens[REGISTRY_INT_PATH_MAX_LEN] = { 0 };
        int res = 0;
        bool exit_folder_iteration = false;

        while (exit_folder_iteration == false) {
            int dir_entry_position = -1;
            do {
                res = vfs_readdir(&dirp, &dir_entry);
                dir_entry_position++;

                if (dir_entry_position > last_dir_entry_positions[i]) {
                    last_dir_entry_positions[i] = dir_entry_position;
                    for (size_t j = i + 1; j < REGISTRY_INT_PATH_MAX_LEN; j++) {
                        last_dir_entry_positions[j] = -1;
                    }

                    if (res == 1) {
                        if (strcmp(dir_entry.d_name,
                                   ".") != 0 &&
                            strcmp(dir_entry.d_name, "..") != 0) {
                            /* save string_path length to restore it later */
                            last_dir_string_path_lens[i] = strlen(string_path);

                            /* add new directory to string_path */
                            strcat(string_path, "/");
                            strcat(string_path, dir_entry.d_name);

                            vfs_stat(string_path, &_stat);

                            if (S_ISDIR(_stat.st_mode)) {
                                /* close old directory */
                                if (vfs_closedir(&dirp) != 0) {
                                    DEBUG(
                                        "[registry storage_vfs] load: Can not close dir\n");
                                }

                                /* open new directory */
                                if (vfs_opendir(&dirp, string_path) != 0) {
                                    DEBUG("[registry storage_vfs] load: Can not open dir\n");
                                }

                                /* move on to next sub path */
                                i++;

                                /* reset position within current dir, because the dir changed */
                                dir_entry_position = -1;
                            }
                            else {
                                /* open file */
                                int fd = vfs_open(string_path, O_RDONLY, 0);

                                if (fd <= 0) {
                                    DEBUG(
                                        "[registry storage_vfs] load: Can not open file: %d\n",
                                        fd);
                                }

                                /* convert string path of integers to native registry int path format (remove mount point and first '/' character) */
                                char *ptr = (char *)string_path + mount->mount_point_len + 1;
                                registry_namespace_id_t namespace_id = strtol(ptr, &ptr, 10);
                                ptr++;
                                registry_schema_id_t schema_id = strtol(ptr, &ptr, 10);
                                ptr++;
                                registry_instance_id_t instance_id = strtol(ptr, &ptr, 10);
                                ptr++;
                                registry_parameter_id_t parameter_id = strtol(ptr, &ptr, 10);

                                const registry_int_path_t int_path = {
                                    .type = REGISTRY_INT_PATH_TYPE_PARAMETER,
                                    .value.parameter_path = {
                                        .namespace_id = namespace_id,
                                        .schema_id = schema_id,
                                        .instance_id = instance_id,
                                        .parameter_id = parameter_id,
                                    }
                                };

                                /* get pointer to registry internal configuration parameter */
                                registry_node_t node;
                                registry_node_from_int_path(&int_path, &node);

                                /* get value from registry to know its size (buf_len) */
                                registry_value_t value;
                                registry_get(&node, &value);

                                /* read value from file */
                                uint8_t new_value_buf[value.buf_len];
                                if (vfs_read(fd, new_value_buf, value.buf_len) < 0) {
                                    DEBUG(
                                        "[registry storage_vfs] load: Can not read from file\n");
                                }
                                else {
                                    /* call callback with value and path */
                                    load_cb(&node, new_value_buf, value.buf_len);
                                }

                                /* close file */
                                if (vfs_close(fd) != 0) {
                                    DEBUG(
                                        "[registry storage_vfs] load: Can not close file: %d\n",
                                        fd);
                                }

                                /* restore old string_path */
                                string_path[last_dir_string_path_lens[i]] = '\0';
                            }
                        }
                    }
                    else {
                        /* if i == 0 it can't be further decreased => exit */
                        if (i == 0) {
                            exit_folder_iteration = true;
                        }
                        else {
                            /* move up one path back to the parent */
                            i--;

                            /* restore old string_path */
                            string_path[last_dir_string_path_lens[i]] = '\0';

                            /* close old directory */
                            if (vfs_closedir(&dirp) != 0) {
                                DEBUG("[registry storage_vfs] load: Can not close dir\n");
                            }

                            /* open new directory */
                            if (vfs_opendir(&dirp, string_path) != 0) {
                                DEBUG("[registry storage_vfs] load: Can not open dir\n");
                            }
                        }
                    }
                }
            } while (res == 1);
        }

        if (vfs_closedir(&dirp) != 0) {
            DEBUG("[registry storage_vfs] load: Can not close dir\n");
        }
    }

    /* umount */
    _umount(mount);

    return REGISTRY_ERROR_NONE;
}

static registry_error_t save(
    const registry_storage_instance_t *storage,
    const registry_node_t *node,
    const registry_value_t *value)
{
    assert(node->type == REGISTRY_NODE_PARAMETER);
    assert(node->value.parameter.parameter != NULL);
    assert(node->value.parameter.instance != NULL);

    vfs_mount_t *mount = storage->data;

    /* mount */
    _mount(mount);

    /* create dir path */
    registry_int_path_t path;
    int res = registry_node_to_int_path(node, &path);

    char string_path[REGISTRY_INT_PATH_STRING_MAX_LEN];

    sprintf(string_path, "%s", mount->mount_point);

    _string_path_append_item(string_path, path.value.parameter_path.namespace_id);
    res = vfs_mkdir(string_path, 0);

    if (res < 0 && res != -EEXIST) {
        DEBUG("[registry storage_vfs] save: Can not make dir: %s\n", string_path);
    }

    _string_path_append_item(string_path, path.value.parameter_path.schema_id);
    res = vfs_mkdir(string_path, 0);

    if (res < 0 && res != -EEXIST) {
        DEBUG("[registry storage_vfs] save: Can not make dir: %s\n", string_path);
    }

    _string_path_append_item(string_path, path.value.parameter_path.instance_id);
    res = vfs_mkdir(string_path, 0);

    if (res < 0 && res != -EEXIST) {
        DEBUG("[registry storage_vfs] save: Can not make dir: %s\n", string_path);
    }

    /* open file */
    _string_path_append_item(string_path, path.value.parameter_path.parameter_id);

    int fd = vfs_open(string_path, O_CREAT | O_RDWR, 0);

    if (fd <= 0) {
        DEBUG("[registry storage_vfs] save: Can not open file: %d\n", fd);
    }

    /* write to file */
    if (vfs_write(fd, value->buf, value->buf_len) < 0) {
        DEBUG("[registry storage_vfs] save: Can not write to file: %d\n", fd);
    }

    if (vfs_close(fd) != 0) {
        DEBUG("[registry storage_vfs] save: Can not close file: %d\n", fd);
    }

    /* umount */
    _umount(mount);

    return REGISTRY_ERROR_NONE;
}
