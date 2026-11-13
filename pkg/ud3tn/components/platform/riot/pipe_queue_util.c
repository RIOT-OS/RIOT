// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#include "platform/posix/pipe_queue_util.h"

#include <unistd.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

ssize_t pipeq_read_all(int fd, void *buf, size_t count)
{
    uint8_t *tmp_buf = (uint8_t *)buf;
    ssize_t read_bytes = 0;

    for (;;) {
        ssize_t tmp_result = read(fd, tmp_buf, count);

        if ((size_t)tmp_result == count) {
            return read_bytes + tmp_result;
        }
        else if (tmp_result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                errno == EINTR) {
                continue;
            }
            return -1;
        }
        // unlikely case
        read_bytes += tmp_result;
        tmp_buf += tmp_result;
        count -= tmp_result;
    }
}

ssize_t pipeq_write_all(int fd, const void *buf, size_t count)
{
    const uint8_t *tmp_buf = (uint8_t *)buf;
    ssize_t written_bytes = 0;

    for (;;) {
        ssize_t tmp_result = write(fd, tmp_buf, count);

        if ((size_t)tmp_result == count) {
            return written_bytes + tmp_result;
        }
        else if (tmp_result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                errno == EINTR) {
                continue;
            }
            return -1;
        }
        // unlikely case
        written_bytes += tmp_result;
        tmp_buf += tmp_result;
        count -= tmp_result;
    }
}
