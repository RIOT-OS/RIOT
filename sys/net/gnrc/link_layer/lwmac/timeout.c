/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Timeout handling of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <errno.h>

#include "net/gnrc/lwmac/timeout.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
char *lwmac_timeout_names[] = {
    [GNRC_LWMAC_TIMEOUT_DISABLED]              = "DISABLED",
    [GNRC_LWMAC_TIMEOUT_WR]                    = "WR",
    [GNRC_LWMAC_TIMEOUT_NO_RESPONSE]           = "NO_RESPONSE",
    [GNRC_LWMAC_TIMEOUT_DATA]                  = "DATA",
    [GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP]      = "WAIT_FOR_DEST_WAKEUP",
    [GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD]         = "WAKEUP_PERIOD",
};
#endif

static inline void _lwmac_clear_timeout(gnrc_lwmac_timeout_t *timeout)
{
    assert(timeout);

    xtimer_remove(&(timeout->timer));
    timeout->type = GNRC_LWMAC_TIMEOUT_DISABLED;
}

/* Return index >= 0 if found, -ENONENT if not found */
static int _lwmac_find_timeout(gnrc_lwmac_t *lwmac, gnrc_lwmac_timeout_type_t type)
{
    assert(lwmac);

    for (unsigned i = 0; i < GNRC_LWMAC_TIMEOUT_COUNT; i++) {
        if (lwmac->timeouts[i].type == type) {
            return i;
        }
    }
    return -ENOENT;
}

inline bool gnrc_lwmac_timeout_is_running(gnrc_netdev_t *gnrc_netdev,
                                          gnrc_lwmac_timeout_type_t type)
{
    assert(gnrc_netdev);
    return (_lwmac_find_timeout(&gnrc_netdev->lwmac, type) >= 0);
}

bool gnrc_lwmac_timeout_is_expired(gnrc_netdev_t *gnrc_netdev, gnrc_lwmac_timeout_type_t type)
{
    assert(gnrc_netdev);

    int index = _lwmac_find_timeout(&gnrc_netdev->lwmac, type);
    if (index >= 0) {
        if (gnrc_netdev->lwmac.timeouts[index].expired) {
            _lwmac_clear_timeout(&gnrc_netdev->lwmac.timeouts[index]);
        }
        return gnrc_netdev->lwmac.timeouts[index].expired;
    }
    return false;
}

gnrc_lwmac_timeout_t *_lwmac_acquire_timeout(gnrc_netdev_t *gnrc_netdev,
                                             gnrc_lwmac_timeout_type_t type)
{
    assert(gnrc_netdev);

    if (gnrc_lwmac_timeout_is_running(gnrc_netdev, type)) {
        return NULL;
    }

    for (unsigned i = 0; i < GNRC_LWMAC_TIMEOUT_COUNT; i++) {
        if (gnrc_netdev->lwmac.timeouts[i].type == GNRC_LWMAC_TIMEOUT_DISABLED) {
            gnrc_netdev->lwmac.timeouts[i].type = type;
            return &gnrc_netdev->lwmac.timeouts[i];
        }
    }
    return NULL;
}

void gnrc_lwmac_timeout_make_expire(gnrc_lwmac_timeout_t *timeout)
{
    assert(timeout);

    timeout->expired = true;
}

void gnrc_lwmac_clear_timeout(gnrc_netdev_t *gnrc_netdev, gnrc_lwmac_timeout_type_t type)
{
    assert(gnrc_netdev);

    int index = _lwmac_find_timeout(&gnrc_netdev->lwmac, type);
    if (index >= 0) {
        _lwmac_clear_timeout(&gnrc_netdev->lwmac.timeouts[index]);
    }
}

void gnrc_lwmac_set_timeout(gnrc_netdev_t *gnrc_netdev,
                            gnrc_lwmac_timeout_type_t type,
                            uint32_t offset)
{
    assert(gnrc_netdev);

    gnrc_lwmac_timeout_t *timeout;
    if ((timeout = _lwmac_acquire_timeout(gnrc_netdev, type))) {
        DEBUG("[LWMAC] Set timeout %s in %" PRIu32 " us\n",
              lwmac_timeout_names[type], offset);
        timeout->expired = false;
        timeout->msg.type = GNRC_LWMAC_EVENT_TIMEOUT_TYPE;
        timeout->msg.content.ptr = (void *) timeout;
        xtimer_set_msg(&(timeout->timer), offset,
                       &(timeout->msg), gnrc_netdev->pid);
    }
    else {
        DEBUG("[LWMAC] Cannot set timeout %s, too many concurrent timeouts\n",
              lwmac_timeout_names[type]);
    }
}

void gnrc_lwmac_reset_timeouts(gnrc_netdev_t *gnrc_netdev)
{
    assert(gnrc_netdev);

    for (unsigned i = 0; i < GNRC_LWMAC_TIMEOUT_COUNT; i++) {
        if (gnrc_netdev->lwmac.timeouts[i].type != GNRC_LWMAC_TIMEOUT_DISABLED) {
            _lwmac_clear_timeout(&gnrc_netdev->lwmac.timeouts[i]);
        }
    }
}
