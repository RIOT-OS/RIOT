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

#define SHELL_VFS_BUFSIZE 256
static uint8_t _shell_vfs_data_buffer[SHELL_VFS_BUFSIZE];

static void _ls_usage(char **argv)
{
    printf("%s <path>\n", argv[0]);
    puts("list files in <path>");
}

static void _vfs_usage(char **argv)
{
    printf("%s <r|w> <path> [bytes] [offset]\n", argv[0]);
    printf("%s ls <path>\n", argv[0]);
    printf("%s cp <src> <dest>\n", argv[0]);
    printf("%s mv <src> <dest>\n", argv[0]);
    printf("%s rm <file>\n", argv[0]);
    printf("%s df [path]\n", argv[0]);
    puts("r: Read [bytes] bytes at [offset] in file <path>");
    puts("w: not implemented yet");
    puts("ls: list files in <path>");
    puts("mv: Move <src> file to <dest>");
    puts("cp: Copy <src> file to <dest>");
    puts("rm: Unlink (delete) <file>");
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

static void _print_df(const char *path)
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

static int _df_handler(int argc, char **argv)
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

static int _read_handler(int argc, char **argv)
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
        memset(buf, 0, sizeof(buf));
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
        for (int k = res; k < sizeof(buf); ++k) {
            if ((k % 2) == 0) {
                putchar(' ');
            }
            putchar(' ');
            putchar(' ');
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

static int _cp_handler(int argc, char **argv)
{
    char errbuf[16];
    if (argc < 3) {
        _vfs_usage(argv);
        return 1;
    }
    char *src_name = argv[1];
    char *dest_name = argv[2];
    printf("%s: copy src: %s dest: %s\n", argv[0], src_name, dest_name);

    int fd_in = vfs_open(src_name, O_RDONLY, 0);
    if (fd_in < 0) {
        _errno_string(fd_in, (char *)errbuf, sizeof(errbuf));
        printf("Error opening file for reading \"%s\": %s\n", src_name, errbuf);
        return 2;
    }
    int fd_out = vfs_open(dest_name, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_out < 0) {
        _errno_string(fd_out, (char *)errbuf, sizeof(errbuf));
        printf("Error opening file for writing \"%s\": %s\n", dest_name, errbuf);
        return 2;
    }
    int eof = 0;
    while (eof == 0) {
        size_t bufspace = sizeof(_shell_vfs_data_buffer);
        size_t pos = 0;
        while (bufspace > 0) {
            int res = vfs_read(fd_in, &_shell_vfs_data_buffer[pos], bufspace);
            if (res < 0) {
                _errno_string(res, (char *)errbuf, sizeof(errbuf));
                printf("Error reading %lu bytes @ 0x%lx in \"%s\" (%d): %s\n",
                    (unsigned long)bufspace, (unsigned long)pos, src_name, fd_in, errbuf);
                vfs_close(fd_in);
                vfs_close(fd_out);
                return 2;
            }
            if (res == 0) {
                /* EOF */
                eof = 1;
                break;
            }
            if (res > bufspace) {
                printf("READ BUFFER OVERRUN! %d > %lu\n", res, (unsigned long)bufspace);
                vfs_close(fd_in);
                vfs_close(fd_out);
                return 3;
            }
            pos += res;
            bufspace -= res;
        }
        bufspace = pos;
        pos = 0;
        while (bufspace > 0) {
            int res = vfs_write(fd_out, &_shell_vfs_data_buffer[pos], bufspace);
            if (res <= 0) {
                _errno_string(res, (char *)errbuf, sizeof(errbuf));
                printf("Error writing %lu bytes @ 0x%lx in \"%s\" (%d): %s\n",
                    (unsigned long)bufspace, (unsigned long)pos, dest_name, fd_out, errbuf);
                vfs_close(fd_in);
                vfs_close(fd_out);
                return 4;
            }
            if (res > bufspace) {
                printf("WRITE BUFFER OVERRUN! %d > %lu\n", res, (unsigned long)bufspace);
                vfs_close(fd_in);
                vfs_close(fd_out);
                return 5;
            }
            bufspace -= res;
        }
    }
    printf("Copied: %s -> %s\n", src_name, dest_name);
    vfs_close(fd_in);
    vfs_close(fd_out);
    return 0;
}

static int _mv_handler(int argc, char **argv)
{
    if (argc < 3) {
        _vfs_usage(argv);
        return 1;
    }
    char *src_name = argv[1];
    char *dest_name = argv[2];
    printf("%s: move src: %s dest: %s\n", argv[0], src_name, dest_name);

    int res = vfs_rename(src_name, dest_name);
    if (res < 0) {
        char errbuf[16];
        _errno_string(res, (char *)errbuf, sizeof(errbuf));
        printf("mv ERR: %s\n", errbuf);
        return 2;
    }
    return 0;
}

static int _rm_handler(int argc, char **argv)
{
    if (argc < 2) {
        _vfs_usage(argv);
        return 1;
    }
    char *rm_name = argv[1];
    printf("%s: unlink: %s\n", argv[0], rm_name);

    int res = vfs_unlink(rm_name);
    if (res < 0) {
        char errbuf[16];
        _errno_string(res, (char *)errbuf, sizeof(errbuf));
        printf("rm ERR: %s\n", errbuf);
        return 2;
    }
    return 0;
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
    else if (strcmp(argv[1], "ls") == 0) {
        return _ls_handler(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "cp") == 0) {
        return _cp_handler(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "mv") == 0) {
        return _mv_handler(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "rm") == 0) {
        return _rm_handler(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "df") == 0) {
        return _df_handler(argc - 1, &argv[1]);
    }
    else {
        printf("vfs: unsupported sub-command \"%s\"\n", argv[1]);
        return 1;
    }
}
#endif
