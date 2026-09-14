/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_vfs_util    VFS helper functions
 * @ingroup     sys_vfs
 * @{
 *
 * @file
 * @brief       VFS helper functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Writes the content of a buffer to a file
 *          If the file already exists, it will be overwritten.
 *
 * @param[in]  file     Destination file path
 * @param[in]  buf      Source buffer
 * @param[in]  len      Buffer size
 *
 * @return  0 on success
 * @return  negative error from @ref vfs_open, @ref vfs_write
 */
int vfs_file_from_buffer(const char *file, const void *buf, size_t len);

/**
 * @brief   Reads the content of a file to a buffer
 *
 * @param[in]  file     Source file path
 * @param[out] buf      Destination buffer
 * @param[in]  len      Buffer size
 *
 * @return  number of bytes read on success
 * @return  -ENOSPC if the file was read successfully but is larger than
 *          the provided buffer. Only the first @p len bytes were read.
 * @return  negative error from @ref vfs_open, @ref vfs_read
 */
int vfs_file_to_buffer(const char* file, void* buf, size_t len);

#if MODULE_HASHES || DOXYGEN
/**
 * @brief   Compute the MD5 message digest of a file
 *
 *          Requires the `hashes` module.
 *
 * @param[in]  file     Source file path
 * @param[out] digest   Destination buffer, must fit @ref MD5_DIGEST_LENGTH bytes
 * @param[out] work_buf Work buffer
 * @param[in] work_buf_len  Size of the work buffer
 *
 * @return  0 on success
 * @return  negative error
 */
int vfs_file_md5(const char* file, void *digest,
                 void *work_buf, size_t work_buf_len);

/**
 * @brief   Compute the SHA1 message digest of a file
 *
 *          Requires the `hashes` module.
 *
 * @param[in]  file     Source file path
 * @param[out] digest   Destination buffer, must fit @ref SHA1_DIGEST_LENGTH bytes
 * @param[out] work_buf Work buffer
 * @param[in] work_buf_len  Size of the work buffer
 *
 * @return  0 on success
 * @return  negative error
 */
int vfs_file_sha1(const char* file, void *digest,
                  void *work_buf, size_t work_buf_len);

/**
 * @brief   Compute the SHA256 message digest of a file
 *
 *          Requires the `hashes` module.
 *
 * @param[in]  file     Source file path
 * @param[out] digest   Destination buffer, must fit @ref SHA256_DIGEST_LENGTH bytes
 * @param[out] work_buf Work buffer
 * @param[in] work_buf_len  Size of the work buffer
 *
 * @return  0 on success
 * @return  negative error
 */
int vfs_file_sha256(const char* file, void *digest,
                    void *work_buf, size_t work_buf_len);
#endif

/**
 * @brief   Checks if @p path is a file or a directory.
 *
 * This function uses @ref vfs_stat(), so if you need @ref vfs_stat() anyway,
 * you should not do double work and check it yourself.
 *
 * @param[in]   path        Path to check
 *
 * @return < 0 on FS error
 * @return 0 if @p path is a file
 * @return > 0 if @p path is a directory
 */
int vfs_is_dir(const char *path);

/**
 @brief Checks if @p path is a file and can be read.
 *
 * @param[in]   path        Path to check
 *
 * @return true if the file exists, false otherwise
 */
bool vfs_file_exists(const char *path);

/**
 * @brief    Behaves like `rm -r @p root`.
 *
 * @param[in]   root        FS root directory to be deleted
 * @param[in]   path_buf    Buffer that must be able to store the longest path
 *                          of the file or directory being deleted
 * @param[in]   max_size    Size of @p path_buf
 *
 * @return < 0 on error
 * @return 0
 */
int vfs_unlink_recursive(const char *root, char *path_buf, size_t max_size);

#ifdef __cplusplus
}
#endif

/** @} */
