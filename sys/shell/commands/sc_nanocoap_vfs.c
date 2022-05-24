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

#include "vfs_default.h"
#include "net/nanocoap_vfs.h"
#include "net/nanocoap_sock.h"

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

static int _print_dir_cb(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    (void)offset;
    (void)more;

    struct dir_list_ctx *ctx = arg;

    char *end = (char *)buf + len;
    for (char *c = (char *)buf; c < end; ++c) {
        if (ctx->cur) {
            if (*c == '>' || ctx->cur == ctx->end) {
                *ctx->cur = 0;
                puts(ctx->buf);
                ctx->cur = NULL;
            } else {
                *ctx->cur++ = *c;
            }
        } else if (*c == '<') {
            ctx->cur = ctx->buf;
        }
    }

    return 0;
}

static int _print_dir(const char *url, char *buf, size_t len)
{
    struct dir_list_ctx ctx = {
        .buf = buf,
        .end = buf + len,
    };
    return nanocoap_get_blockwise_url(url, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                      _print_dir_cb, &ctx);
}

int _nanocoap_get_handler(int argc, char **argv)
{
    int res;
    char buffer[CONFIG_NANOCOAP_QS_MAX];
    char *dst, *url = argv[1];

    if (argc < 2) {
        printf("Usage: %s <url> [destination]\n", argv[0]);
        printf("Default destination: %s\n", VFS_DEFAULT_DATA);
        return -EINVAL;
    }

    if (_is_dir(url) && argc < 3) {
        res = _print_dir(url, buffer, sizeof(buffer));
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
                     VFS_DEFAULT_DATA, dst) >= (int)sizeof(buffer)) {
            printf("Output file path too long\n");
            return -ENOBUFS;
        }
        dst = buffer;
    } else {
        dst = argv[2];
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
