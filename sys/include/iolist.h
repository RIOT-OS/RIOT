/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 * @return iolist_size(iolist) on success
 * @retval -ENOBUFS if the buffer is too small
 */
ssize_t iolist_to_buffer(const iolist_t *iolist, void *dst, size_t len);

/**
 * @brief   Copies the bytes from a buffer to the iolist
 *
 * @param[out]  iolist  iolist to read from
 * @param[in]   src     Source buffer
 * @param[in]   len     Size of the source buffer
 *
 * @return @p len on success
 * @retval -ENOBUFS if the iolist is too small
 */
ssize_t iolist_from_buffer(iolist_t *iolist, const void *src, size_t len);

#ifdef __cplusplus
}
#endif
/** @} */
