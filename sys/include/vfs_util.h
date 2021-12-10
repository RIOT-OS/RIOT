/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

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

#ifndef VFS_UTIL_H
#define VFS_UTIL_H

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

#ifdef __cplusplus
}
#endif

#endif /* VFS_UTIL_H */
/** @} */
