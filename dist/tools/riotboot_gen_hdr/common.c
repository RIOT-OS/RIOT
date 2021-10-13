/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

/**
 * @file
 * @brief       Common tools for RIOT images header generation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int to_file(const char *filename, void *buf, size_t len)
{
    int fd;

    if (strcmp("-", filename)) {
        fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    }
    else {
        fd = STDOUT_FILENO;
    }

    if (fd > 0) {
        ssize_t res = write(fd, buf, len);
        close(fd);
        return res == (ssize_t)len;
    }
    else {
        return fd;
    }
}
