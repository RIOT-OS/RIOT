/*
 * Copyright (C) 2019 Freie Universität Berlin
 *               2019 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit
 * @{
 *
 * @file
 * @brief       SUIT transport worker thread
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>

#include "msg.h"
#include "log.h"
#include "thread.h"
#include "time_units.h"
#include "periph/pm.h"
#include "ztimer.h"

#ifdef MODULE_SUIT_TRANSPORT_COAP
#include "net/nanocoap_sock.h"
#include "suit/transport/coap.h"
#include "net/sock/util.h"
#endif
#ifdef MODULE_SUIT_TRANSPORT_VFS
#include "vfs_util.h"
#endif

#ifdef MODULE_RIOTBOOT_SLOT
#include "riotboot/slot.h"
#endif

#ifdef MODULE_SUIT
#include "suit.h"
#include "suit/handlers.h"
#include "suit/storage.h"
#endif

#if defined(MODULE_PROGRESS_BAR)
#include "progress_bar.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef SUIT_WORKER_STACKSIZE
/* allocate stack needed to do manifest validation */
#define SUIT_WORKER_STACKSIZE (3 * THREAD_STACKSIZE_LARGE)
#endif

#ifndef SUIT_COAP_WORKER_PRIO
#define SUIT_COAP_WORKER_PRIO THREAD_PRIORITY_MAIN - 1
#endif

#ifndef SUIT_MANIFEST_BUFSIZE
#define SUIT_MANIFEST_BUFSIZE   640
#endif

#define SUIT_MSG_TRIGGER        0x12345

static char _stack[SUIT_WORKER_STACKSIZE];
static char _url[CONFIG_SOCK_URLPATH_MAXLEN];
static uint8_t _manifest_buf[SUIT_MANIFEST_BUFSIZE];

static kernel_pid_t _suit_worker_pid;

static void _suit_handle_url(const char *url)
{
    ssize_t size;
    LOG_INFO("suit_worker: downloading \"%s\"\n", url);

    if (0) {}
#ifdef MODULE_SUIT_TRANSPORT_COAP
    else if (strncmp(url, "coap://", 7) == 0) {
        size = nanocoap_get_blockwise_url_to_buf(url,
                                                 CONFIG_SUIT_COAP_BLOCKSIZE,
                                                 _manifest_buf,
                                                 sizeof(_manifest_buf));
    }
#endif
#ifdef MODULE_SUIT_TRANSPORT_VFS
    else if (strncmp(url, "file://", 7) == 0) {
        size = vfs_file_to_buffer(&url[7], _manifest_buf, sizeof(_manifest_buf));
    }
#endif
    else {
        LOG_WARNING("suit_worker: unsupported URL scheme!\n)");
        return;
    }

    if (size >= 0) {
        LOG_INFO("suit_worker: got manifest with size %u\n", (unsigned)size);

#ifdef MODULE_SUIT
        suit_manifest_t manifest;
        memset(&manifest, 0, sizeof(manifest));

        manifest.urlbuf = _url;
        manifest.urlbuf_len = CONFIG_SOCK_URLPATH_MAXLEN;

        int res;
        if ((res = suit_parse(&manifest, _manifest_buf, size)) != SUIT_OK) {
            LOG_INFO("suit_worker: suit_parse() failed. res=%i\n", res);
            return;
        }

#endif
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
        if (res == 0) {
            const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(
                riotboot_slot_other());
            riotboot_hdr_print(hdr);
            ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

            if (riotboot_hdr_validate(hdr) == 0) {
                LOG_INFO("suit_worker: rebooting...\n");
                pm_reboot();
            }
            else {
                LOG_INFO("suit_worker: update failed, hdr invalid\n ");
            }
        }
#endif
    }
    else {
        LOG_INFO("suit_worker: error getting manifest\n");
    }
}

static void *_suit_worker_thread(void *arg)
{
    (void)arg;

    LOG_INFO("suit_worker: started.\n");
    msg_t msg_queue[4];
    msg_init_queue(msg_queue, 4);

    _suit_worker_pid = thread_getpid();

    msg_t m;
    while (true) {
        msg_receive(&m);
        DEBUG("suit_worker: got msg with type %" PRIu32 "\n", m.content.value);
        switch (m.content.value) {
            case SUIT_MSG_TRIGGER:
                LOG_INFO("suit_worker: trigger received\n");
                _suit_handle_url(_url);
                break;
            default:
                LOG_WARNING("suit_worker: warning: unhandled msg\n");
        }
    }
    return NULL;
}

void suit_coap_run(void)
{
    thread_create(_stack, SUIT_WORKER_STACKSIZE, SUIT_COAP_WORKER_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _suit_worker_thread, NULL, "suit worker");
}

void suit_coap_trigger(const uint8_t *url, size_t len)
{
    memcpy(_url, url, len);
    _url[len] = '\0';
    msg_t m = { .content.value = SUIT_MSG_TRIGGER };
    msg_send(&m, _suit_worker_pid);
}
