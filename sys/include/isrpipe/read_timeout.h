/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    isr_pipe_read_timeout  Read timeouts with ISR pipe
 * @ingroup     isr_pipe
 * @brief       ISR -> userspace pipe with timeout
 *
 * @{
 * @file
 * @brief       isrpipe read timeout Interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include "isrpipe.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Read data from isrpipe (with timeout, blocking)
 *
 * Currently, the timeout parameter is applied on every underlying read, which
 * might be *per single byte*.
 *
 * @note This function might return less than @p count bytes
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 * @param[in]   buf        buffer to write to
 * @param[in]   count      number of bytes to read
 * @param[in]   timeout    timeout in microseconds
 *
 * @returns     number of bytes read
 * @returns     -ETIMEDOUT on timeout
 */
int isrpipe_read_timeout(isrpipe_t *isrpipe, uint8_t *buf, size_t count, uint32_t timeout);

/**
 * @brief   Read data from isrpipe (with timeout, blocking, wait until all read)
 *
 * This function is like @ref isrpipe_read_timeout, but will only return on
 * timeout or when @p count bytes have been received.
 *
 * A timeout only occurs if no data is received within @p timeout microseconds.
 * Each time a byte or burst of bytes is received, the timeout is reset.
 *
 * @note If a timeout occurs, some bytes may have been read out of the pipe and
 * placed into @p buf.
 *
 * @param[in]   isrpipe    isrpipe object to operate on
 * @param[in]   buf        buffer to write to
 * @param[in]   count      number of bytes to read
 * @param[in]   timeout    timeout in microseconds
 *
 * @returns     number of bytes read
 * @returns     -ETIMEDOUT on timeout
 */
int isrpipe_read_all_timeout(isrpipe_t *isrpipe, uint8_t *buf, size_t count, uint32_t timeout);

#ifdef __cplusplus
}
#endif
/** @} */
