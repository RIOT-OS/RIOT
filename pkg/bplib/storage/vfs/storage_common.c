/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_bplib_storage_vfs
 *
 * @{
 *
 * @file
 * @brief       Common functions of VFS based storage
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "storage_common.h"

#include <fcntl.h>

#include "vfs.h"
#include "bplib_init.h"

bplib_stor_common_data_t bplib_stor_common_data;

bool bplib_stor_vfs_bundle_path_from_id(uint32_t bundle_id, char* path_dat)
{
    char path_idx[BPLIB_STOR_PATHLEN_IDX] = BPLIB_STOR_PATH_INDEX;
    ssize_t bytes_read;
    int fd;
    uint64_t node, service, time;
    bool failed = false;

    sprintf(path_idx + BPLIB_STOR_INDEX_LEN, "/%" PRIx32, bundle_id);
    fd = vfs_open(path_idx, O_RDONLY, 0777);
    if (fd < 0) {
        /* Index does not exist */
        return false;
    }

    bytes_read = vfs_read(fd, &node, sizeof(uint64_t));
    if (bytes_read != sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

    bytes_read = vfs_read(fd, &service, sizeof(uint64_t));
    if (bytes_read != sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

    bytes_read = vfs_read(fd, &time, sizeof(uint64_t));
    if (bytes_read != sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

close_file:
    vfs_close(fd);

    if (!failed) {
        sprintf(path_dat, BPLIB_STOR_PATH_DATA "/%" PRIx64 "/%" PRIx64 "/%" PRIx64 "_%" PRIx32,
            node, service, time, bundle_id);
    }

    return !failed;
}

void bplib_stor_vfs_delete_bundle(uint32_t bundle_id)
{
    char path[BPLIB_STOR_PATHLEN_MAX];
    int rv;
    bool success = false;
    struct stat stat;
    BPLib_Instance_t* inst = &bplib_instance_data.BPLibInst;

    success = bplib_stor_vfs_bundle_path_from_id(bundle_id, path);

    if (success) {
        /* The read results are valid, can delete the data file */
        rv = vfs_stat(path, &stat);

        /* Only decrement the storage counters if the data file actually exists. This function
         * is also called when storing fails, so in this case it should not decrease. */
        if (rv == 0) {
            inst->BundleStorage.BytesStorageInUse -= stat.st_size + BUNDLE_STORAGE_INDEX_SIZE;
            inst->BundleStorage.BundleCountStored--;
        }
        vfs_unlink(path);
    }

    sprintf(path, BPLIB_STOR_PATH_INDEX "/%" PRIx32, bundle_id);
    vfs_unlink(path);
}

bool bplib_stor_vfs_update_retransmission_time(const char* path,
                                                      uint32_t retrans_interval)
{
    int fd;
    ssize_t status;
    uint64_t next_retransmission;
    uint8_t flags;
    bool res = true;
    bool flags_changed = false;

    fd = vfs_open(path, O_RDWR, 0777);
    if (fd < 0) {
        return false;
    }

    status = vfs_lseek(fd, offsetof(bundle_file_structure_t, header.flags), SEEK_SET);
    if (status < 0) {
        res = false;
        goto close_file;
    }

    status = vfs_read(fd, &flags, sizeof(uint8_t));
    if (status < 0 || status != sizeof(uint8_t)) {
        res = false;
        goto close_file;
    }

    if (retrans_interval != 0) {
        status = vfs_lseek(fd, offsetof(bundle_file_structure_t, header.retransmit_timestamp),
                            SEEK_SET);
        if (status < 0) {
            res = false;
            goto close_file;
        }

        next_retransmission = retrans_interval + BPLib_TIME_GetMonotonicTime();

        status = vfs_write(fd, &next_retransmission, sizeof(uint64_t));
        if (status < 0 || status != (ssize_t) sizeof(uint64_t)) {
            res = false;
            goto close_file;
        }

        /* Retransmissions are currently off => Turn them on */
        if (flags & STORAGE_HEADER_FLAG_RETRANS_OFF) {
            flags &= ~STORAGE_HEADER_FLAG_RETRANS_OFF;
            flags_changed = true;
        }
    }
    else {
        /* Retransmissions are currently on => Turn them off */
        if (!(flags & STORAGE_HEADER_FLAG_RETRANS_OFF)) {
            flags |= STORAGE_HEADER_FLAG_RETRANS_OFF;
            flags_changed = true;
        }
    }

    /* Write back flags, common for both, but only if the flags changed */
    if (flags_changed) {
        status = vfs_lseek(fd, offsetof(bundle_file_structure_t, header.flags), SEEK_SET);
        if (status < 0) {
            res = false;
            goto close_file;
        }

        status = vfs_write(fd, &flags, sizeof(uint8_t));
        if (status < 0 || status != (ssize_t) sizeof(uint8_t)) {
            res = false;
            goto close_file;
        }
    }

close_file:
    if (vfs_close(fd) < 0) {
        res = false;
    }

    return res;
}
