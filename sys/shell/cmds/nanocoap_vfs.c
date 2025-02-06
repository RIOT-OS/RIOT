/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
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
 * @brief       NanoCoAP commands that interact with the filesystem
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "net/nanocoap/link_format.h"
#include "net/nanocoap_sock.h"
#include "net/nanocoap_vfs.h"

#include "shell.h"
#include "vfs_default.h"
#include "vfs_util.h"

/**
 * @brief   Default download location for ncget
 */
#ifndef CONFIG_NCGET_DEFAULT_DATA_DIR
#define CONFIG_NCGET_DEFAULT_DATA_DIR VFS_DEFAULT_DATA
#endif

struct dir_list_ctx {
    char *buf;
    char *cur;
    char *end;
};

static bool _is_dir(const char *url)
{
    int len = strlen(url);
    return url[len - 1] == '/';
}

static int _resource_cb(char *entry, void *ctx)
{
    bool *too_long = ctx;
    char *start, *end;

    if (*too_long) {
        goto find_end;
    }

    start = strchr(entry, '<');
    if (start == NULL) {
        return 0;
    }
    entry = start + 1;

find_end:
    end = strchr(entry, '>');
    if (end == NULL) {
        *too_long = true;
        printf("%s", entry);
    }
    else {
        *too_long = false;
        *end = '\0';
        puts(entry);
    }

    return 0;
}

static int _print_cb(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    (void)arg;
    (void)offset;

    write(STDOUT_FILENO, buf, len);
    if (!more) {
        puts("");
    }

    return 0;
}

static int _nanocoap_get_handler(int argc, char **argv)
{
    int res;
    char buffer[CONFIG_NANOCOAP_URI_MAX];
    char *dst, *url = argv[1];

    if (argc < 2) {
        printf("Usage: %s <url> [destination]\n", argv[0]);
        printf("Default destination: %s\n", CONFIG_NCGET_DEFAULT_DATA_DIR);
        return -EINVAL;
    }

    if (_is_dir(url) && argc < 3) {
        bool _ctx = false;
        res = nanocoap_link_format_get_url(url, _resource_cb, &_ctx);
        if (res) {
            printf("Request failed: %s\n", strerror(-res));
        }
        return res;
    }

    if (argc < 3) {
        dst = strrchr(url, '/');
        if (dst == NULL) {
            printf("invalid url: '%s'\n", url);
            return -EINVAL;
        }
        if (snprintf(buffer, sizeof(buffer), "%s%s",
                     CONFIG_NCGET_DEFAULT_DATA_DIR, dst) >= (int)sizeof(buffer)) {
            printf("Output file path too long\n");
            return -ENOBUFS;
        }
        dst = buffer;
    } else {
        char *filename = strrchr(url, '/');
        dst = argv[2];
        if (vfs_is_dir(dst) > 0 && filename) {
            if (snprintf(buffer, sizeof(buffer), "%s%s",
                         dst, filename) >= (int)sizeof(buffer)) {
                printf("Output file path too long\n");
                return -ENOBUFS;
            }
            dst = buffer;
        }
    }

    /* alternatively write the file to stdout */
    if (strcmp(dst, "-") == 0) {
        return nanocoap_get_blockwise_url(url, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                          _print_cb, NULL);
    }

    res = nanocoap_vfs_get_url(url, dst);
    if (res < 0) {
        printf("Download failed: %s\n", strerror(-res));
    } else {
        printf("Saved as %s\n", dst);
    }
    return res;
}

static int _nanocoap_put_handler(int argc, char **argv)
{
    int res;
    char *file, *url;
    char buffer[CONFIG_NANOCOAP_URI_MAX];
    char work_buf[coap_szx2size(CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT) + 1];

    if (argc < 3) {
        printf("Usage: %s <file> <url>\n", argv[0]);
        return -EINVAL;
    }

    file = argv[1];
    url = argv[2];

    /* append filename to remote URL */
    if (_is_dir(url)) {
        const char *basename = strrchr(file, '/');
        if (basename == NULL) {
            basename = file;
        } else {
            ++basename;
        }

        if (snprintf(buffer, sizeof(buffer), "%s%s",
                     url, basename) >= (int)sizeof(buffer)) {
            puts("Constructed URI too long");
            return -ENOBUFS;
        }
        url = buffer;
    }

    /* file path is relative to CONFIG_NCGET_DEFAULT_DATA_DIR */
    if (file[0] != '/') {
        if ((unsigned)snprintf(work_buf, sizeof(work_buf), "%s%s",
                     CONFIG_NCGET_DEFAULT_DATA_DIR, file) >= sizeof(work_buf)) {
            puts("Constructed URI too long");
            return -ENOBUFS;
        }
        file = work_buf;
    }

    /* read from 'stdin' / 3rd argument */
    if (strcmp(file, "-") == 0) {
        if (argc < 4) {
            printf("Usage: %s - <url> <data>\n", argv[0]);
            return -EINVAL;
        }
        res = nanocoap_sock_put_url(url, argv[3], strlen(argv[3]), NULL, 0);
    } else {
        res = nanocoap_vfs_put_url(url, file, work_buf, sizeof(work_buf));
    }

    if (res < 0) {
        printf("Upload failed: %s\n", strerror(-res));
    }
    else {
        printf("Saved to %s\n", url);
    }
    return res;
}

SHELL_COMMAND(ncget, "download a file from a CoAP server", _nanocoap_get_handler);
SHELL_COMMAND(ncput, "upload a file to a CoAP server", _nanocoap_put_handler);
