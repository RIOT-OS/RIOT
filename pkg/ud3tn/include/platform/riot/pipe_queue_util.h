/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief Helpers for reading from and writing to queue pipes.
 */
#ifndef PIPE_QUEUE_UTIL_H_INCLUDED
#define PIPE_QUEUE_UTIL_H_INCLUDED

#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>

/**
 * @brief Read all requested bytes from a file descriptor.
 *
 * @param fd The file descriptor to be read from.
 * @param buf The buffer used for storing read data.
 * @param count The amount of bytes to be read.
 * @return The amount of bytes read, or -1 on error. Note that when this
 *         function returns an error (-1), some bytes might have been read but
 *         were discarded.
 */
ssize_t pipeq_read_all(int fd, void *buf, size_t count);

/**
 * @brief Write all requested bytes to a file descriptor.
 *
 * @param fd The file descriptor to be written to.
 * @param buf The buffer containing the data to be written.
 * @param count The amount of bytes to be written.
 * @return The amount of bytes written, or -1 on error. Note that when this
 *         function returns an error (-1), some bytes might have been written
 *         already.
 */
ssize_t pipeq_write_all(int fd, const void *buf, size_t count);

#endif /* PIPE_QUEUE_UTIL_H_INCLUDED */
