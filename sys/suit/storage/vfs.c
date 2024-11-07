/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT vfs storage module implementation
 *
 *
 * @}
 */
#include <string.h>
#include <inttypes.h>
#include <fcntl.h>

#include "board.h"

#include "xfa.h"
#include "fmt.h"
#include "kernel_defines.h"
#include "log.h"

#include "vfs.h"
#include "suit.h"
#include "suit/storage.h"
#include "suit/storage/vfs.h"

XFA_USE(suit_storage_t, suit_storage_reg);
XFA_INIT(char *, suit_storage_files_reg);

static inline suit_storage_vfs_t *_get_vfs(suit_storage_t *storage)
{
    return container_of(storage, suit_storage_vfs_t, storage);
}

__attribute__((unused))
static inline const suit_storage_vfs_t *_get_vfs_const(const suit_storage_t *storage)
{
    return container_of(storage, suit_storage_vfs_t, storage);
}

static inline const char *_get_active_file(suit_storage_vfs_t *vfs)
{
    return vfs->files[vfs->active_region];
}

static int _vfs_update_seq_no(uint32_t seq_no)
{
    char buf[16];
    uint32_t sequence_no = 0;
    int res = SUIT_ERR_SEQUENCE_NUMBER;

    int fd = vfs_open(CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        LOG_INFO("ERROR: failed to open %s\n", CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION);
        return res;
    }

    if (vfs_read(fd, buf, strlen("4294967295")) > 0) {
        sequence_no = strtoul(buf, NULL, 0);
    }

    if (sequence_no < seq_no) {
        ssize_t len = fmt_u32_dec(buf, seq_no);
        vfs_lseek(fd, 0, SEEK_SET);
        if (vfs_write(fd, buf, len) == len) {
            res = SUIT_OK;
            LOG_DEBUG("Stored sequence number: %" PRIu32 "\n", seq_no);
        }
        else {
            LOG_INFO("ERROR: failed to write seq_no %" PRIu32 " to %s\n", seq_no,
                     CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION);
        }
    }
    vfs_close(fd);
    return res;
}

static int _vfs_init(suit_storage_t *storage)
{
    suit_storage_vfs_t *vfs = _get_vfs(storage);

    vfs->files = (const char **)suit_storage_files_reg;

    return SUIT_OK;
}

static int _vfs_start(suit_storage_t *storage, const suit_manifest_t *manifest,
                      size_t len)
{
    (void)manifest;
    (void)len;
    (void)storage;

    return SUIT_OK;
}

static int _vfs_write(suit_storage_t *storage, const suit_manifest_t *manifest,
                      const uint8_t *buf, size_t offset, size_t len)
{
    (void)manifest;
    suit_storage_vfs_t *vfs = _get_vfs(storage);
    const char *filepath = _get_active_file(vfs);

    int fd = vfs_open(filepath, O_RDWR | O_CREAT, 0);

    /* seek to the given offset */
    int rc = vfs_lseek(fd, offset, SEEK_SET);

    if (rc < 0) {
        return rc;
    }
    else if ((size_t)rc != offset) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    /* write all bytes to the file */
    rc = vfs_write(fd, buf, len);
    if (rc < 0) {
        return rc;
    }
    else if ((size_t)rc != len) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    vfs_close(fd);
    return SUIT_OK;
}

static int _vfs_finish(suit_storage_t *storage, const suit_manifest_t *manifest)
{
    (void)manifest;
    (void)storage;
    return SUIT_OK;
}

static int _vfs_install(suit_storage_t *storage, const suit_manifest_t *manifest)
{
    (void)storage;
    return _vfs_update_seq_no(manifest->seq_number);
}

static int _vfs_erase(suit_storage_t *storage)
{
    suit_storage_vfs_t *vfs = _get_vfs(storage);
    const char *filepath = _get_active_file(vfs);

    vfs_unlink(filepath);
    return SUIT_OK;
}

static int _vfs_read(suit_storage_t *storage, uint8_t *buf, size_t offset,
                     size_t len)
{
    suit_storage_vfs_t *vfs = _get_vfs(storage);
    const char *filepath = _get_active_file(vfs);

    int fd = vfs_open(filepath, O_RDWR | O_CREAT, 0);

    /* seek to the given offset */
    int rc = vfs_lseek(fd, offset, SEEK_SET);

    if (rc < 0) {
        return rc;
    }
    else if ((size_t)rc != offset) {
        return SUIT_ERR_STORAGE;
    }

    /* read from file into the buffer */
    rc = vfs_read(fd, buf, len);
    if (rc < 0) {
        return rc;
    }
    else if ((size_t)rc != len) {
        return SUIT_ERR_STORAGE;
    }

    vfs_close(fd);

    return SUIT_OK;
}

static bool _get_region_by_string(const char *location, uint32_t *val)
{
    for (size_t i = 0; i < XFA_LEN(char **, suit_storage_files_reg); i++) {
        const char *filepath = (const char *)suit_storage_files_reg[i];
        if (strncmp(filepath, location, strlen(filepath)) == 0) {
            *val = i;
            return true;
        }
    }

    return false;
}

static int _vfs_set_active_location(suit_storage_t *storage,
                                    const char *location)
{
    suit_storage_vfs_t *vfs = _get_vfs(storage);
    uint32_t region = 0;

    if (!_get_region_by_string(location, &region)) {
        return -1;
    }

    vfs->active_region = region;

    return 0;
}

static bool _vfs_has_location(const suit_storage_t *storage, const char *location)
{
    (void)storage;
    uint32_t region = 0;

    return _get_region_by_string(location, &region);
}

static int _vfs_get_seq_no(const suit_storage_t *storage, uint32_t *seq_no)
{
    (void)storage;

    char buf[16];

    int fd = vfs_open(CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        LOG_INFO("ERROR: failed to open %s\n", CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION);
        return SUIT_ERR_SEQUENCE_NUMBER;
    }

    if (vfs_read(fd, buf, strlen("4294967295")) > 0) {
        *seq_no = strtoul(buf, NULL, 0);
    }
    LOG_INFO("Retrieved sequence number: %" PRIu32 "\n", *seq_no);
    vfs_close(fd);
    return SUIT_OK;
}

static int _vfs_set_seq_no(suit_storage_t *storage, uint32_t seq_no)
{
    (void)storage;

    return _vfs_update_seq_no(seq_no);
}

const suit_storage_driver_t suit_storage_vfs_driver = {
    .init = _vfs_init,
    .start = _vfs_start,
    .write = _vfs_write,
    .finish = _vfs_finish,
    .read = _vfs_read,
    .install = _vfs_install,
    .erase = _vfs_erase,
    .set_active_location = _vfs_set_active_location,
    .has_location = _vfs_has_location,
    .get_seq_no = _vfs_get_seq_no,
    .set_seq_no = _vfs_set_seq_no,
    .separator = '\0',
};

suit_storage_vfs_t suit_storage_vfs = {
    .storage = {
        .driver = &suit_storage_vfs_driver,
    },
};

XFA(suit_storage_t *, suit_storage_reg, 0) suit_storage_vfs_ptr = &suit_storage_vfs.storage;
