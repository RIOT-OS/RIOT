/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "fs/devfs.h"
#include "vfs.h"
#include "mutex.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/**
 * @internal
 * @brief DevFS list head
 *
 * DevFS operates as a singleton, the same files show up in all mounted instances.
 */
static clist_node_t _devfs_list;
/**
 * @internal
 * @brief mutex to protect the DevFS list from corruption
 */
static mutex_t _devfs_mutex = MUTEX_INIT;

/* No need for file system operations, no extra work to be done on
 * mount/umount. unlink is not permitted, use devfs_unregister instead */

/* File operations */
/* open is overloaded to allow searching for the correct device */
static int devfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path);
/* A minimal fcntl is also provided to enable SETFL handling */
static int devfs_fcntl(vfs_file_t *filp, int cmd, int arg);

/* Directory operations */
static int devfs_opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path);
static int devfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry);
static int devfs_closedir(vfs_DIR *dirp);

static const vfs_file_ops_t devfs_file_ops = {
    .open  = devfs_open,
    .fcntl = devfs_fcntl,
};

static const vfs_dir_ops_t devfs_dir_ops = {
    .opendir = devfs_opendir,
    .readdir = devfs_readdir,
    .closedir = devfs_closedir,
};

const vfs_file_system_t devfs_file_system = {
    .f_op = &devfs_file_ops,
    .d_op = &devfs_dir_ops,
};

static int devfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    DEBUG("devfs_open: %p, \"%s\", 0x%x, 0%03lo, \"%s\"\n", (void *)filp, name, flags, (unsigned long)mode, abs_path);
    /* linear search through the device list */
    mutex_lock(&_devfs_mutex);
    clist_node_t *it = _devfs_list.next;
    if (it == NULL) {
        /* list empty */
        mutex_unlock(&_devfs_mutex);
        return -ENOENT;
    }
    do {
        it = it->next;
        devfs_t *devp = container_of(it, devfs_t, list_entry);
        if (strcmp(devp->path, name) == 0) {
            mutex_unlock(&_devfs_mutex);
            DEBUG("devfs_open: Found :)\n");
            /* Add private data from DevFS node */
            filp->private_data.ptr = devp->private_data;
            /* Replace f_op with the operations provided by the device driver */
            filp->f_op = devp->f_op;
            /* Chain the open() method for the specific device */
            if (filp->f_op->open != NULL) {
                return filp->f_op->open(filp, name, flags, mode, abs_path);
            }
            return 0;
        }
    } while (it != _devfs_list.next);
    mutex_unlock(&_devfs_mutex);
    DEBUG("devfs_open: Not found :(\n");
    return -ENOENT;
}

static int devfs_fcntl(vfs_file_t *filp, int cmd, int arg)
{
    DEBUG("devfs_fcntl: %p, 0x%x, 0x%x\n", (void *)filp, cmd, arg);
    switch (cmd) {
        /* F_GETFL is handled directly by vfs_fcntl */
        case F_SETFL:
            DEBUG("devfs_fcntl: SETFL: %d\n", arg);
            filp->flags = arg;
            return filp->flags;
        default:
            return -EINVAL;
    }
}

static int devfs_opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    (void) abs_path;
    DEBUG("devfs_opendir: %p, \"%s\", \"%s\"\n", (void *)dirp, dirname, abs_path);
    if (strncmp(dirname, "/", 2) != 0) {
        /* We keep it simple and only support a flat file system, only a root directory */
        return -ENOENT;
    }
    dirp->private_data.ptr = _devfs_list.next;
    return 0;
}

static int devfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    DEBUG("devfs_readdir: %p, %p\n", (void *)dirp, (void *)entry);
    mutex_lock(&_devfs_mutex);
    clist_node_t *it = dirp->private_data.ptr;
    if (it == NULL) {
        /* empty list */
        mutex_unlock(&_devfs_mutex);
        return 0;
    }
    it = it->next;
    if (it == _devfs_list.next) {
        /* end of list was reached */
        mutex_unlock(&_devfs_mutex);
        return 0;
    }
    dirp->private_data.ptr = it;
    mutex_unlock(&_devfs_mutex);
    devfs_t *devp = container_of(it, devfs_t, list_entry);
    if (devp->path == NULL) {
        /* skip past the broken entry and try again */
        return -EAGAIN;
    }
    size_t len = strnlen(devp->path, VFS_NAME_MAX + 1);
    if (len > VFS_NAME_MAX) {
        /* name does not fit in vfs_dirent_t buffer */
        /* skip past the broken entry and try again */
        return -EAGAIN;
    }
    /* clear the dirent */
    memset(entry, 0, sizeof(*entry));
    /* copy the string, including terminating null */
    memcpy(&entry->d_name[0], devp->path, len + 1);
    return 1;
}

static int devfs_closedir(vfs_DIR *dirp)
{
    /* Just an example, it's not necessary to define closedir if there is
     * nothing to clean up */
    (void) dirp;
    DEBUG("devfs_closedir: %p\n", (void *)dirp);
    return 0;
}

int devfs_register(devfs_t *devp)
{
    DEBUG("devfs_register: %p\n", (void *)devp);
    if (devp == NULL) {
        return -EINVAL;
    }
    DEBUG("devfs_register: \"%s\" -> (%p, %p)\n", devp->path, (void *)devp->f_op, devp->private_data);
    if (devp->path == NULL) {
        return -EINVAL;
    }
    if (devp->f_op == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_devfs_mutex);
    clist_node_t *it = _devfs_list.next;
    if (it != NULL) {
        /* list not empty */
        do {
            it = it->next;
            if (it == &devp->list_entry) {
                /* Already registered */
                mutex_unlock(&_devfs_mutex);
                return -EEXIST;
            }
            devfs_t *devit = container_of(it, devfs_t, list_entry);
            if (strcmp(devit->path, devp->path) == 0) {
                /* Path already registered */
                mutex_unlock(&_devfs_mutex);
                return -EEXIST;
            }
        } while(it != _devfs_list.next);
    }
    /* insert last in list */
    clist_rpush(&_devfs_list, &devp->list_entry);
    mutex_unlock(&_devfs_mutex);
    return 0;
}

int devfs_unregister(devfs_t *devp)
{
    DEBUG("devfs_unregister: %p\n", (void *)devp);
    if (devp == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_devfs_mutex);
    /* find devp in the list and remove it */
    clist_node_t *node = clist_remove(&_devfs_list, &devp->list_entry);
    mutex_unlock(&_devfs_mutex);
    if (node == NULL) {
        /* not found */
        DEBUG("devfs_unregister: ERR not registered!\n");
        return -ENOENT;
    }
    return 0;
}
