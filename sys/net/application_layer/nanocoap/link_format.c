/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       NanoCoAP Link Format parser
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "fmt.h"
#include "net/nanocoap/link_format.h"
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"

struct dir_list_ctx {
    char *buf;
    char *cur;
    char *end;
    coap_link_format_handler_t cb;
    void *ctx;
    char esc_buf[2];
    uint8_t esc_idx;
};

static int _dirlist_cb(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    (void)offset;

    struct dir_list_ctx *ctx = arg;

    char *end = (char *)buf + len;
    for (char *c = (char *)buf; c != end; ++c) {

        /* start of escape sequence */
        if (*c == '%') {
            ctx->esc_idx = 1;
            continue;
        }
        if (ctx->esc_idx) {
            /* fill escape buffer */
            ctx->esc_buf[ctx->esc_idx - 1] = *c;
            if (++ctx->esc_idx == 3) {
                ctx->esc_idx = 0;
                *c = scn_u32_hex(ctx->esc_buf, 2);
            } else {
                continue;
            }
        }

        bool found_end = false;
        if (*c == ',') {
            found_end = true;
        }
        else {
            *ctx->cur++ = *c;
        }

        if (found_end || ctx->cur == ctx->end) {
            *ctx->cur = 0;
            int res = ctx->cb(ctx->buf, ctx->ctx);
            ctx->cur = ctx->buf;
            if (res < 0) {
                return res;
            }
        }
    }

    if (!more) {
        *ctx->cur = 0;
        return ctx->cb(ctx->buf, ctx->ctx);
    }

    return 0;
}

int nanocoap_link_format_get(nanocoap_sock_t *sock, const char *path,
                             coap_link_format_handler_t cb, void *arg,
                             char *dirent_buf, size_t dirent_buf_len)
{
    struct dir_list_ctx ctx = {
        .buf = dirent_buf,
        .end = dirent_buf + dirent_buf_len,
        .cur = dirent_buf,
        .cb = cb,
        .ctx = arg,
    };
    return nanocoap_sock_get_blockwise(sock, path, CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT,
                                       _dirlist_cb, &ctx);
}

int nanocoap_link_format_get_url(const char *url, coap_link_format_handler_t cb, void *arg,
                                 char *dirent_buf, size_t dirent_buf_len)
{
    nanocoap_sock_t sock;
    int res = nanocoap_sock_url_connect(url, &sock);
    if (res) {
        return res;
    }

    res = nanocoap_link_format_get(&sock, sock_urlpath(url), cb, arg, dirent_buf, dirent_buf_len);
    nanocoap_sock_close(&sock);

    return res;
}
