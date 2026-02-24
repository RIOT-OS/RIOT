/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_iolist iolist scatter / gather IO
 * @ingroup     sys
 * @brief       Provides linked-list scatter / gather IO
 *
 * @{
 *
 * @file
 * @brief       iolist scatter / gather IO
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief iolist forward declaration */
typedef struct iolist iolist_t;

/**
 * @brief   iolist structure definition
 */
struct iolist {
    iolist_t *iol_next;     /**< ptr to next list entry */
    void *iol_base;         /**< ptr to this list entries data */
    size_t iol_len;         /**< size of data pointet to by ptr */
};

/**
 * @brief   Count number of entries in an iolist_t
 *
 * @param[in]   iolist  iolist to count
 *
 * @returns number of entries (zero for NULL parameter)
 */
unsigned iolist_count(const iolist_t *iolist);

/**
 * @brief   Sum up number of bytes in iolist
 *
 * This function returns the summed ip length values of all entries in @p
 * iolist.
 *
 * @param[in]   iolist  iolist to sum up
 *
 * @returns summed up number of bytes or zero if @p iolist == NULL
 */
size_t iolist_size(const iolist_t *iolist);

/** @brief  struct iovec anonymous declaration */
struct iovec;

/**
 * @brief   Create struct iovec from iolist
 *
 * This function fills an array of struct iovecs with the contents of @p
 * iolist. It will write the number of used array entries into @p count.
 *
 * The caller *must* ensure that @p iov p points to an array of size >= count!
 *
 * @param[in]   iolist  iolist to read from
 * @param[out]  iov     ptr to array of struct iovec that will be filled
 * @param[out]  count   number of elements in @p iolist
 *
 * @returns iolist_size(iolist)
 */
size_t iolist_to_iovec(const iolist_t *iolist, struct iovec *iov, unsigned *count);

/**
 * @brief   Copies the bytes of the iolist to a buffer
 *
 * @param[in]   iolist  iolist to read from
 * @param[out]  dst     Destination buffer
 * @param[in]   len     Size of the destination buffer
 *
 * @returns iolist_size(iolist) on success
 *          -ENOBUFS if the buffer is too small
 */
ssize_t iolist_to_buffer(const iolist_t *iolist, void *dst, size_t len);

#ifdef __cplusplus
}
#endif
/** @} */
