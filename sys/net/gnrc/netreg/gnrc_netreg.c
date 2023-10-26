/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>
#include <string.h>
#include <limits.h>

#include "assert.h"
#include "log.h"
#include "utlist.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#ifdef MODULE_GNRC_TCP
#include "net/gnrc/tcp.h"
#endif

#define _INVALID_TYPE(type) (((type) < GNRC_NETTYPE_UNDEF) || ((type) >= GNRC_NETTYPE_NUMOF))

/* The registry as lookup table by gnrc_nettype_t */
static gnrc_netreg_entry_t *netreg[GNRC_NETTYPE_NUMOF];

/** Held while accessing _lock_counter, and also while the exclusive lock is held */
static mutex_t _lock_for_counter = MUTEX_INIT;
/** Number of shared locks on netreg. Saturating arithmetic is used; if this
 * reaches UINT_MAX, the lock will never be freed again. This is likely
 * accurate, given that it will only happen when the lock was leaked, so it
 * can't be freed any more anyway.
 *
 * This can be accessed only when _lock_for_counter is held; an alternative
 * implementation with atomics would likely be possible, but for the case of "I
 * can't do that right now" it would still need a mutex. But that's
 * optimization that could be done without changing the public API.
 * */
static unsigned int _lock_counter = 0;
/** Held while the netreg lists are being read. This is what the exclusive
 * users block on if they can't grab the exclusive lock right away.
 *
 * This is largely used as a boolean flag (is locked / is unlocked) that is
 * modified while the _lock_for_counter is held, and will not block because it
 * is synchronized to _lock_counter being 0. It is only ever accessed outside
 * _lock_for_counter when waiting for the counter to reach 0, and even then is
 * released immediately to avoid deadlocks. (Instead, the exclusive acquisition
 * tries to acquire the _lock_for_counter, and if that fails it queues up again
 * after the reader that just snatched it).
 * */
static mutex_t _lock_wait_exclusive = MUTEX_INIT;

void gnrc_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, GNRC_NETTYPE_NUMOF * sizeof(gnrc_netreg_entry_t *));
}

void gnrc_netreg_acquire_shared(void) {
    mutex_lock(&_lock_for_counter);
    if (_lock_counter == 0) {
        /* At most, this blocks for the very short time until
         * _gnrc_netreg_acquire_exclusive returns it immediately */
        mutex_lock(&_lock_wait_exclusive);
    }
    if (_lock_counter != UINT_MAX) {
        _lock_counter += 1;
    }
    mutex_unlock(&_lock_for_counter);
}

void gnrc_netreg_release_shared(void) {
    mutex_lock(&_lock_for_counter);

    assert(_lock_counter != 0); /* Release without acquire */

    if (_lock_counter != UINT_MAX) {
        _lock_counter -= 1;
    }
    if (_lock_counter == 0) {
        mutex_unlock(&_lock_wait_exclusive);
    }
    mutex_unlock(&_lock_for_counter);
}

/** Assert that there is a shared lock on gnrc_netreg -- this should help weed
 * out callers to @ref gnrc_netreg_lookup that don't properly lock. */
static void _gnrc_netreg_assert_shared(void) {
#if DEVELHELP
    /* Even if we just peek: It's not an atomic, so it needs synchronization */
    mutex_lock(&_lock_for_counter);
    assert(_lock_counter != 0);
    mutex_unlock(&_lock_for_counter);
#endif
}

static void _gnrc_netreg_acquire_exclusive(void) {
    while (true) {
        mutex_lock(&_lock_for_counter);
        if (_lock_counter == 0) {
            /* At most, this blocks for the very short time until
             * another caller of _gnrc_netreg_acquire_exclusive returns it
             * immediately */
            mutex_lock(&_lock_wait_exclusive);
            /* Leaving both locked */
            return;
        }
        mutex_unlock(&_lock_for_counter);

        mutex_lock(&_lock_wait_exclusive);
        /* ... but maybe someone just started grabbing _lock_for_counter, so we
         * give them a chance to finish rather than deadlocking them */
        mutex_unlock(&_lock_wait_exclusive);
    }
}

static void _gnrc_netreg_release_exclusive(void) {
     mutex_unlock(&_lock_wait_exclusive);
     mutex_unlock(&_lock_for_counter);
}

int gnrc_netreg_register(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
#if DEVELHELP
    thread_t *thread = thread_get(entry->target.pid);
    assert(thread);

# if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
    bool has_msg_q = (entry->type != GNRC_NETREG_TYPE_DEFAULT) ||
                     thread_has_msg_queue(thread);
# else
    bool has_msg_q = thread_has_msg_queue(thread);
# endif

    /* only threads with a message queue are allowed to register at gnrc */
    if (!has_msg_q) {
        LOG_ERROR("\n!!!! gnrc_netreg: initialize message queue of thread %u "
                  "using msg_init_queue() !!!!\n\n", entry->target.pid);
    }
    assert(has_msg_q);
#endif /* DEVELHELP */

    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    _gnrc_netreg_acquire_exclusive();

    /* don't add the same entry twice */
    gnrc_netreg_entry_t *e;
    LL_FOREACH(netreg[type], e) {
        assert(entry != e);
    }

    LL_PREPEND(netreg[type], entry);
    _gnrc_netreg_release_exclusive();

    return 0;
}

void gnrc_netreg_unregister(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return;
    }

    _gnrc_netreg_acquire_exclusive();
    LL_DELETE(netreg[type], entry);
    /* We can release now already: No new references to this entry can be made
     * any more, and the caller is only allowed to reuse the entry and the mbox
     * target referenced by it after *this* function returned, not when the
     * lock becomes available again. */
    _gnrc_netreg_release_exclusive();

#if defined(MODULE_GNRC_NETAPI_MBOX)
    /* drain packets still in the mbox */
    if (entry->type == GNRC_NETREG_TYPE_MBOX) {
        msg_t msg;
        while (mbox_try_get(entry->target.mbox, &msg)) {
            if ((msg.type == GNRC_NETAPI_MSG_TYPE_RCV) ||
                (msg.type == GNRC_NETAPI_MSG_TYPE_SND)) {
                gnrc_pktbuf_release_error(msg.content.ptr, EBADF);
            }
        }
    }
#endif
}

/**
 * @brief   Searches the next entry in the registry that matches given
 *          parameters, start lookup from beginning or given entry.
 *
 * @param[in] from      A registry entry to lookup from or NULL to start fresh
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  The first entry fitting the given parameters on success
 * @return  NULL if no entry can be found.
 */
static gnrc_netreg_entry_t *_netreg_lookup(gnrc_netreg_entry_t *from,
                                           gnrc_nettype_t type,
                                           uint32_t demux_ctx)
{
    _gnrc_netreg_assert_shared();

    gnrc_netreg_entry_t *res = NULL;

    if (from || !_INVALID_TYPE(type)) {
        gnrc_netreg_entry_t *head = (from) ? from->next : netreg[type];
        LL_SEARCH_SCALAR(head, res, demux_ctx, demux_ctx);
    }

    return res;
}

gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx)
{
    return _netreg_lookup(NULL, type, demux_ctx);
}

int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    gnrc_netreg_entry_t *entry = NULL;

    gnrc_netreg_acquire_shared();

    while((entry = _netreg_lookup(entry, type, demux_ctx)) != NULL) {
        num++;
    }

    gnrc_netreg_release_shared();

    return num;
}

gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry)
{
    return (entry ? _netreg_lookup(entry, 0, entry->demux_ctx) : NULL);
}

int gnrc_netreg_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr)
{
    if (pseudo_hdr == NULL) {
        /* XXX: Might be allowed for future checksums.
         *      If this is the case: move this to the branches were it
         *      is needed. */
        return -EINVAL;
    }

    switch (hdr->type) {
#ifdef MODULE_GNRC_ICMPV6
        case GNRC_NETTYPE_ICMPV6:
            return gnrc_icmpv6_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_GNRC_TCP
        case GNRC_NETTYPE_TCP:
            return gnrc_tcp_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_GNRC_UDP
        case GNRC_NETTYPE_UDP:
            return gnrc_udp_calc_csum(hdr, pseudo_hdr);
#endif
        default:
            return -ENOENT;
    }
}

/** @} */
