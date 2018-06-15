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
/**
 * @brief   CryptoFS Magic word
 */
#define CRYPTOFS_MAGIC_WORD (0xAB560001)
#endif

/**
 * @name    CryptoFS file header parameters
 */
#define CRYPTOFS_NBPAD_OFFSET       (4)
#define CRYPTOFS_MODE_OFFSET        (5)
#define CRYPTOFS_HEAD_HASH_OFFSET   (16)
#define CRYPTOFS_HEAD_SIZE          (48)
/** @} */

#ifndef CRYPTOFS_OPENDIR_MAX
/**
 * @brief   Maximum number of directory that can be opened
 */
#define CRYPTOFS_OPENDIR_MAX        (1)
#endif

#ifndef CRYPTOFS_ROOT_FILENAME
/**
 * @brief   CryptoFS superblock file name
 */
#define CRYPTOFS_ROOT_FILENAME      ".cryptofs0"
#endif

#ifndef CRYPTOFS_BLOCK_SIZE
/**
 * @brief   Internal caching size, must be a multiple of AES_BLOCK_SIZE
 */
#define CRYPTOFS_BLOCK_SIZE         (4 * AES_BLOCK_SIZE)
#endif

#if CRYPTOFS_BLOCK_SIZE % AES_BLOCK_SIZE != 0
#error "CRYPTOFS_BLOCK_SIZE must be a multiple of AES_BLOCK_SIZE"
#endif

/*
 * File header:
 * +-------------------------------------------------------+
 * | MAGIC_WORD (4B) | nb_pad (1B) | mode (1B) | RFU (10B) |
 * +-------------------------------------------------------+
 * | HASH (32B)                                            |
 * +-------------------------------------------------------+
 * | Encrypted data block #1 (32B)                         |
 * +-------------------------------------------------------+
 * | ...                                                   |
 * +-------------------------------------------------------+
 *
 */

/**
 * @brief   CryptoFS file encryption modes
 */
enum cryptofs_mode {
    mode_ecb,   /**< ECB */
    mode_ctr,   /**< CTR (not supported yet) */
};

/**
 * @brief   A file in CryptoFS
 */
typedef struct {
    int real_fd;                        /**< fd from underlying FS */
    char name[VFS_NAME_MAX + 1];        /**< name on the underlying FS */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< file hash */
    uint8_t block[CRYPTOFS_BLOCK_SIZE]; /**< internal cache */
    size_t size;                        /**< file size */
    uint8_t state;                      /**< state of the current cache */
    uint8_t nb_pad;                     /**< number of padding bytes */
    enum cryptofs_mode mode;            /**< cipher mode */
} cryptofs_file_t;

/**
 * @brief   CryptoFS file system superblock
 */
typedef struct {
    uint8_t key[AES_KEY_SIZE];          /**< file system encryption key */
    vfs_mount_t *real_fs;               /**< handle to the real file system */
    bool hash;                          /**< hash must be calculated / checked */
    /* private data */
    cipher_t cipher;                    /**< internal: cipher */
    mutex_t lock;                       /**< internal: lock */
    vfs_DIR dir[CRYPTOFS_OPENDIR_MAX];  /**< internal: directory cache */
    int dir_used[CRYPTOFS_OPENDIR_MAX]; /**< internal: directory cache state */
    int root_fd;                        /**< internal: superblock file descriptor */
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
