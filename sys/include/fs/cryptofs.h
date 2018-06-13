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
#include <stdbool.h>

#include "vfs.h"
#include "crypto/aes.h"
#include "crypto/ciphers.h"
#include "hashes/sha256.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CRYPTOFS_MAGIC_WORD
#define CRYPTOFS_MAGIC_WORD (0xAB560001)
#endif

#define CRYPTOFS_NBPAD_OFFSET       (4)
#define CRYPTOFS_HEAD_HASH_OFFSET   (16)
#define CRYPTOFS_HEAD_SIZE          (48)

#ifndef CRYPTOFS_OPENDIR_MAX
#define CRYPTOFS_OPENDIR_MAX        (1)
#endif

#ifndef CRYPTOFS_ROOT_FILENAME
#define CRYPTOFS_ROOT_FILENAME      ".cryptofs0"
#endif

#ifndef CRYPTOFS_BLOCK_SIZE
#define CRYPTOFS_BLOCK_SIZE         (4 * AES_BLOCK_SIZE)
#endif

#if CRYPTOFS_BLOCK_SIZE % AES_BLOCK_SIZE != 0
#error "CRYPTOFS_BLOCK_SIZE must be a multiple of AES_BLOCK_SIZE"
#endif

/*
 * File header:
 * +-------------------------------------------+
 * | MAGIC_WORD (4B) | nb_pad (1B) | RFU (11B) |
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
    int real_fd;
    char name[VFS_NAME_MAX + 1];
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint8_t block[CRYPTOFS_BLOCK_SIZE];
    size_t size;
    uint8_t state;
    uint8_t nb_pad;
} cryptofs_file_t;

/**
 * @brief   CryptoFS file system superblock
 */
typedef struct {
    uint8_t key[AES_KEY_SIZE];
    vfs_mount_t *real_fs;
    bool hash;
    /* private data */
    cipher_t cipher;
    mutex_t lock;
    vfs_DIR dir[CRYPTOFS_OPENDIR_MAX];
    int dir_used[CRYPTOFS_OPENDIR_MAX];
    int root_fd;
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
