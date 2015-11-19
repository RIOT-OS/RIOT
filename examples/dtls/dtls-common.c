/*
 * Copyright (C) 2015 Olaf Bergmann <bergmann@tzi.org>
 * Copyright (C) 2015 Hauke Mehrtens <hauke@hauke-m.de>
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TinyDTLS receiving functions
 *
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "assert.h"

#include "tinydtls_debug.h"
#include "tinydtls.h"
#include "dtls.h"
#include "dtls_app.h"

static uint8 buf[DTLS_MAX_BUF];
static int dtls_handle_read(struct dtls_context_t *ctx)
{
    int *fd;
    session_t session;
    ssize_t len;

    fd = dtls_get_app_data(ctx);

    assert(fd);

    memset(&session, 0, sizeof(session_t));
    session.size = sizeof(session.addr.sin6);
    len = recvfrom(*fd, buf, sizeof(buf), 0, (struct sockaddr*) &session.addr.sin6,
                   &session.size);

    if (len < 0) {
        perror("recvfrom");
        return -1;
    }
    else {
        dtls_debug("got %d bytes from port %d\n", len,
                   ntohs(session.addr.sin6.sin6_port));
        if (sizeof(buf) < (size_t) len) {
            dtls_warn("packet was truncated (%d bytes lost)\n", len - sizeof(buf));
        }
    }

    return dtls_handle_message(ctx, &session, buf, len);
}

msg_t _reader_queue[READER_QUEUE_SIZE];

void *dtls_reader(void *args)
{
    /* calling it twice shouldn't do any harm */
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);
    dtls_context_t *dtls_context = (dtls_context_t*) args;

    while (1) {
        dtls_handle_read(dtls_context);
    }

    dtls_free_context(dtls_context);
    return NULL;
}

