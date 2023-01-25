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
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>

#include "mutex.h"
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

static char _stack[SUIT_WORKER_STACKSIZE];
static char _url[CONFIG_SOCK_URLPATH_MAXLEN];
static uint8_t _manifest_buf[SUIT_MANIFEST_BUFSIZE];

static mutex_t _worker_lock;
/* PID of the worker thread, guarded by */
static kernel_pid_t _worker_pid = KERNEL_PID_UNDEF;

int suit_handle_url(const char *url)
{
    ssize_t size;
    LOG_INFO("suit_worker: downloading \"%s\"\n", url);

    if (0) {}
#ifdef MODULE_SUIT_TRANSPORT_COAP
    else if ((strncmp(url, "coap://", 7) == 0) ||
             (IS_USED(MODULE_NANOCOAP_DTLS) && strncmp(url, "coaps://", 8) == 0)) {
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
        return -ENOTSUP;
    }

    if (size < 0) {
        LOG_INFO("suit_worker: error getting manifest\n");
        return size;
    }

    LOG_INFO("suit_worker: got manifest with size %u\n", (unsigned)size);

#ifdef MODULE_SUIT
    suit_manifest_t manifest;
    memset(&manifest, 0, sizeof(manifest));

    manifest.urlbuf = _url;
    manifest.urlbuf_len = CONFIG_SOCK_URLPATH_MAXLEN;

    int res;
    if ((res = suit_parse(&manifest, _manifest_buf, size)) != SUIT_OK) {
        LOG_INFO("suit_worker: suit_parse() failed. res=%i\n", res);
        return res;
    }
#endif
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(riotboot_slot_other());
    riotboot_hdr_print(hdr);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    return riotboot_hdr_validate(hdr);
#endif

    return res;
}

static void *_suit_worker_thread(void *arg)
{
    (void)arg;

    LOG_INFO("suit_worker: started.\n");

    if (suit_handle_url(_url) == 0) {
        LOG_INFO("suit_worker: update successful\n");
        if (IS_USED(MODULE_SUIT_STORAGE_FLASHWRITE)) {
            LOG_INFO("suit_worker: rebooting...\n");
            pm_reboot();
        }
    }
    else {
        LOG_INFO("suit_worker: update failed, hdr invalid\n ");
    }

    mutex_unlock(&_worker_lock);
    thread_zombify();
    /* Actually unreachable, given we're in a thread */
    return NULL;
}

void suit_worker_trigger(const char *url, size_t len)
{
    mutex_lock(&_worker_lock);
    if (_worker_pid != KERNEL_PID_UNDEF) {
        if (thread_kill_zombie(_worker_pid) != 1) {
            /* This will only happen if the SUIT thread runs on a lower
             * priority than the caller */
            LOG_WARNING("Ignoring SUIT trigger: worker is still busy.\n");
            return;
        }
    }

    memcpy(_url, url, len);
    _url[len] = '\0';

    _worker_pid = thread_create(_stack, SUIT_WORKER_STACKSIZE, SUIT_COAP_WORKER_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _suit_worker_thread, NULL, "suit worker");
}
