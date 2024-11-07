/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_storage_vfs  riotboot vfs storage backend
 * @ingroup     sys_suit_storage
 * @brief       SUIT riotboot firmware storage backend
 *
 * VFS storage can service different files mounted on the filesystem. Serviceable
 * FILES must be registered:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "suit/storage/vfs.h"
 * #include "xfa.h"
 *
 * XFA_USE(char*, suit_storage_files_reg);
 * XFA(char*, suit_storage_files_reg, 0) _firmware_0 = VFS_DEFAULT_DATA "/FW0.TXT";
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Once registered its content may be securely updated via SUIT by specifying the
 * "install-id" as the filepath.
 *
 *  @{
 *
 * @brief       riotboot vfs storage backend functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SUIT_STORAGE_VFS_H
#define SUIT_STORAGE_VFS_H

#include "suit.h"
#include "../../sys/include/vfs.h"
#include "vfs_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Storage location string separators
 */
#ifndef CONFIG_SUIT_STORAGE_MOUNT_POINT
#define CONFIG_SUIT_STORAGE_MOUNT_POINT         VFS_DEFAULT_DATA
#endif

/**
 * @brief Storage location string separators
 */
#ifndef CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION
#define CONFIG_SUIT_STORAGE_SEQ_NO_LOCATION     (CONFIG_SUIT_STORAGE_MOUNT_POINT "/SEQNO.txt")
#endif

/**
 * @brief riotboot vfs SUIT storage context
 */
typedef struct {
    suit_storage_t storage;             /**< parent struct */
    const char **files;                 /**< storage file array */
    uint8_t active_region;              /**< Active file idx to write to */
} suit_storage_vfs_t;

#ifdef __cplusplus
}
#endif

#endif /* SUIT_STORAGE_VFS_H */
/** @} */
