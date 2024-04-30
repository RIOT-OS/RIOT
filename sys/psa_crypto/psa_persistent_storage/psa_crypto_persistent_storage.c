/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Implementation of persistent storage for PSA Crypto keys
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <fcntl.h>
#include "psa/crypto.h"
#include "psa_crypto_slot_management.h"
#include "board.h"
#include "vfs.h"
#include "fs/littlefs2_fs.h"
#include "mtd.h"

#define ENABLE_DEBUG    0
#include "debug.h"

XFA_USE(vfs_mount_t, vfs_mountpoints_xfa);

#define UINT32_T_MAX_STRING_LEN     (10)

/* dir path needs to be as long as mount point + psa_key_id_t as string */
#define STRING_PATH_LEN             (strlen(vfs_mountpoints_xfa[0].mount_point) + \
                                     UINT32_T_MAX_STRING_LEN)

psa_status_t psa_write_encoded_key_slot_to_file(psa_key_id_t id,
                                                     uint8_t* input,
                                                     size_t input_len)
{
    /* dir path needs to be as long as mount point + psa_key_id_t as string */
    char string_path[STRING_PATH_LEN];

    sprintf(string_path, "%s/%d", vfs_mountpoints_xfa[0].mount_point, (int) id);

    /* Check whether file already exists */
    int fd = vfs_open(string_path, O_RDWR, 0);
    if (fd >= 0) {
        DEBUG("[psa_crypto] persist key: key with this ID already exists in storage: %d\n", fd);
        vfs_close(fd);
        return PSA_ERROR_ALREADY_EXISTS;
    }

    /* If file does not exist, create one */
    fd = vfs_open(string_path, O_CREAT | O_RDWR, 0);

    if (fd <= 0) {
        DEBUG("[psa_crypto] persist key: Can not open file: %d\n", fd);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    if (vfs_write(fd, input, input_len) < 0) {
        DEBUG("[psa_crypto] persist key: Can not write to file: %d\n", fd);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    if (vfs_close(fd) != 0) {
        DEBUG("[psa_crypto] persist key: Can not close file: %d\n", fd);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_read_encoded_key_slot_from_file(psa_key_id_t id,
                                                     uint8_t *output,
                                                     size_t output_size,
                                                     size_t *output_data_len)
{
    char string_path[STRING_PATH_LEN];

    sprintf(string_path, "%s/%d", vfs_mountpoints_xfa[0].mount_point, (int) id);

    int fd = vfs_open(string_path, O_RDONLY, 0);

    if (fd <= 0) {
        DEBUG("[psa_crypto] read persisted key: Can not open file: %d\n", fd);
        return (fd == -2 ? PSA_ERROR_DOES_NOT_EXIST : PSA_ERROR_STORAGE_FAILURE);
    }

    ssize_t data_bytes = vfs_read(fd, output, output_size);
    if (data_bytes < 0) {
        DEBUG("[psa_crypto] read persisted key: Can not read from file\n");
        return (fd == -2 ? PSA_ERROR_DOES_NOT_EXIST : PSA_ERROR_STORAGE_FAILURE);
    }

    if (vfs_close(fd) != 0) {

        DEBUG("[psa_crypto] read persisted key: Can not close file: %d\n", fd);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    *output_data_len = data_bytes;
    return PSA_SUCCESS;
}

psa_status_t psa_destroy_persistent_key(psa_key_id_t key_id)
{
    if (psa_key_id_is_volatile(key_id)) {
        DEBUG("[psa_crypto] persist key: ID is volatile\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    char string_path[STRING_PATH_LEN];

    sprintf(string_path, "%s/%d", vfs_mountpoints_xfa[0].mount_point, (int) key_id);

    int fd = vfs_unlink(string_path);
    if (fd < 0) {
        DEBUG("[psa_crypto] destroy persisted key: Can not unlink file: %d\n", fd);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}
