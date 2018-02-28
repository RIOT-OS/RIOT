/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fs_cryptofs CryptoFS encrypted file system
 * @ingroup     sys_fs
 * @brief       File encryption layer on top of any other filesystem
 * *
 * @{
 * @file
 * @brief       CryptoFS public API
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef FS_CRYPTO_H
#define FS_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include "vfs.h"
#include "crypto/aes.h"
#include "crypto/ciphers.h"
#include "hashes/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CRYPTOFS_MAGIC_WORD
#define CRYPTOFS_MAGIC_WORD (0xAB560001)
#endif

#define CRYPTOFS_HEAD_PAD           (28)
#define CRYPTOFS_HEAD_HASH_OFFSET   (32)
#define CRYPTOFS_HEAD_SIZE          (64)

/*
 * File header:
 * +-------------------------------------------+
 * | MAGIC_WORD (4B) | nb_pad (1B) | RFU (27B) |
 * +-------------------------------------------+
 * | HASH (32B)                                |
 * +-------------------------------------------+
 * | Encrypted data block #1 (32B)             |
 * +-------------------------------------------+
 * | ...                                       |
 * +-------------------------------------------+
 *
 */

/**
 * @brief   A file in CryptoFS
 */
typedef struct {
    cipher_t cipher;
    sha256_context_t hash_ctx;
    char name[VFS_NAME_MAX + 1];
    uint8_t hash[SHA256_DIGEST_LENGTH];
    int real_fd;
    uint8_t block[AES_BLOCK_SIZE];
    uint8_t state;
    uint8_t nb_pad;
    size_t size;
} cryptofs_file_t;

/**
 * @brief   CryptoFS file system superblock
 */
typedef struct {
    uint8_t key[AES_KEY_SIZE];
    vfs_mount_t *real_fs;
} cryptofs_t;

/**
 * @brief   CryptoFS file system driver
 *
 * For use with vfs_mount
 */
extern const vfs_file_system_t cryptofs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_CRYPTO_H */
/** @} */
