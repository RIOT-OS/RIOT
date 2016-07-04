/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for the VFS module
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#if MODULE_VFS
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "vfs.h"

static void _ls_usage(char **argv)
{
    printf("%s <path>\n", argv[0]);
    puts("list files in <path>");
}

static void _vfs_usage(char **argv)
{
    printf("%s <r|w> <path> [bytes] [offset]\n", argv[0]);
    printf("%s df [path]\n", argv[0]);
    puts("r: Read [bytes] bytes at [offset] in file <path>");
    puts("w: not implemented yet");
    puts("df: show file system space utilization stats");
}

/* Macro used by _errno_string to expand errno labels to string and print it */
#define _case_snprintf_errno_name(x) \
    case x: \
        res = snprintf(buf, buflen, #x); \
        break

static int _errno_string(int err, char *buf, size_t buflen)
{
    int len = 0;
    int res;
    if (err < 0) {
        res = snprintf(buf, buflen, "-");
        if (res < 0) {
            return res;
        }
        if ((size_t)res <= buflen) {
            buf += res;
            buflen -= res;
        }
        len += res;
        err = -err;
    }
    switch (err) {
        _case_snprintf_errno_name(EACCES);
        _case_snprintf_errno_name(ENOENT);
        _case_snprintf_errno_name(EINVAL);
        _case_snprintf_errno_name(EFAULT);
        _case_snprintf_errno_name(EROFS);
        _case_snprintf_errno_name(EIO);
        _case_snprintf_errno_name(ENAMETOOLONG);
        _case_snprintf_errno_name(EPERM);

        default:
            res = snprintf(buf, buflen, "%d", err);
            break;
    }
    if (res < 0) {
        return res;
    }
    len += res;
    return len;
}
#undef _case_snprintf_errno_name

void _print_df(const char *path)
{
    struct statvfs buf;
    int res = vfs_statvfs(path, &buf);
    printf("%-16s ", path);
    if (res < 0) {
        char err[16];
        _errno_string(res, err, sizeof(err));
        printf("statvfs failed: %s\n", err);
        return;
    }
    printf("%12lu %12lu %12lu %7lu%%\n", (unsigned long)buf.f_blocks,
        (unsigned long)(buf.f_blocks - buf.f_bfree), (unsigned long)buf.f_bavail,
        (unsigned long)(((buf.f_blocks - buf.f_bfree) * 100) / buf.f_blocks));
}

int _df_handler(int argc, char **argv)
{
    puts("Mountpoint              Total         Used    Available     Capacity");
    if (argc > 1) {
        const char *path = argv[1];
        _print_df(path);
    }
    else {
        /* Iterate through all mount points */
        const vfs_mount_t *it = NULL;
        while ((it = vfs_iterate_mounts(it)) != NULL) {
            _print_df(it->mount_point);
        }
    }
    return 0;
}

int _read_handler(int argc, char **argv)
{
    uint8_t buf[16];
    size_t nbytes = sizeof(buf);
    off_t offset = 0;
    char *path = argv[1];
    if (argc < 2) {
        puts("vfs read: missing file name");
        return 1;
    }
    if (argc > 2) {
        nbytes = atoi(argv[2]);
    }
    if (argc > 3) {
        offset = atoi(argv[3]);
    }

    int res;
    res = vfs_normalize_path(path, path, strlen(path) + 1);
    if (res < 0) {
        _errno_string(res, (char *)buf, sizeof(buf));
        printf("Invalid path \"%s\": %s\n", path, buf);
        return 5;
    }

    int fd = vfs_open(path, O_RDONLY, 0);
    if (fd < 0) {
        _errno_string(fd, (char *)buf, sizeof(buf));
        printf("Error opening file \"%s\": %s\n", path, buf);
        return 3;
    }

    res = vfs_lseek(fd, offset, SEEK_SET);
    if (res < 0) {
        _errno_string(res, (char *)buf, sizeof(buf));
        printf("Seek error: %s\n", buf);
        vfs_close(fd);
        return 4;
    }

    while (nbytes > 0) {
        size_t line_len = (nbytes < sizeof(buf) ? nbytes : sizeof(buf));
        res = vfs_read(fd, buf, line_len);
        if (res < 0) {
            _errno_string(res, (char *)buf, sizeof(buf));
            printf("Read error: %s\n", buf);
            vfs_close(fd);
            return 5;
        }
        else if ((size_t)res > line_len) {
            printf("BUFFER OVERRUN! %d > %lu\n", res, (unsigned long)line_len);
            vfs_close(fd);
            return 6;
        }
        else if (res == 0) {
            /* EOF */
            printf("-- EOF --\n");
            break;
        }
        printf("%08lx:", (unsigned long)offset);
        for (int k = 0; k < res; ++k) {
            if ((k % 2) == 0) {
                putchar(' ');
            }
            printf("%02x", buf[k]);
        }
        putchar(' ');
        putchar(' ');
        for (int k = 0; k < res; ++k) {
            if (isprint(buf[k])) {
                putchar(buf[k]);
            }
            else {
                putchar('.');
            }
        }
        puts("");
        offset += res;
        nbytes -= res;
    }

    vfs_close(fd);
    return 0;
}

int _vfs_handler(int argc, char **argv)
{
    if (argc < 2) {
        _vfs_usage(argv);
        return 1;
    }
    if (strcmp(argv[1], "r") == 0) {
        /* pass on to read handler, shifting the arguments by one */
        return _read_handler(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "df") == 0) {
        return _df_handler(argc - 1, &argv[1]);
    }
    else {
        printf("vfs: unsupported sub-command \"%s\"\n", argv[1]);
        return 1;
    }
}

int _ls_handler(int argc, char **argv)
{
    if (argc < 2) {
        _ls_usage(argv);
        return 1;
    }
    char *path = argv[1];
    uint8_t buf[16];
    int res;
    res = vfs_normalize_path(path, path, strlen(path) + 1);
    if (res < 0) {
        _errno_string(res, (char *)buf, sizeof(buf));
        printf("Invalid path \"%s\": %s\n", path, buf);
        return 5;
    }
    vfs_DIR dir;
    res = vfs_opendir(&dir, path);
    if (res < 0) {
        _errno_string(res, (char *)buf, sizeof(buf));
        printf("vfs_opendir error: %s\n", buf);
        return 1;
    }
    unsigned int nfiles = 0;

    while (1) {
        vfs_dirent_t entry;
        res = vfs_readdir(&dir, &entry);
        if (res < 0) {
            _errno_string(res, (char *)buf, sizeof(buf));
            printf("vfs_opendir error: %s\n", buf);
            if (res == -EAGAIN) {
                /* try again */
                continue;
            }
            return 2;
        }
        if (res == 0) {
            /* end of stream */
            break;
        }
        printf("%s\n", entry.d_name);
        ++nfiles;
    }
    printf("total %u files\n", nfiles);
    return 0;
}
#endif
